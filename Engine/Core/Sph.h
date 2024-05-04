#pragma once
#include "StructuredBuffer.h"

namespace soku {
class Sph {
  public:
    struct Particle {
        Vector3 position = Vector3::Zero;
        Vector3 color = Vector3::Zero;
        Vector3 velocity = Vector3::Zero;
        Vector3 acceleration = Vector3::Zero;
        float width;
        float time = -1.f;
        float mass = 1.f;
        float density = 0.f;
        float pressure = 0.f;
    };
    struct CSG {
        float dt = 1 / 240.f;
        float k = 1.f;
        float friction = 0.1f;
        float rho_0 = 1.f;
    };
    struct CSL {
        float j;
        float f;
        Vector2 dummy;
    };
    struct GeometryConstsCpu {
        float ratio;
        Vector3 dummy;
    };
    Sph() {}
    // Initialize Particles Position, Velocity, Color, etc.
    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    const int &particleCounts);
    // Update Particles
    void Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context, float &dt,
                const float ratio = 1.f);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

    void UpdateDensity();

    void UpdateAcceleration();

    float CubicSplineGrad(const float q);

    float CubicSpline(const float q);
    
    CSG m_cpuGlobal;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_gpuGlobal;
    std::vector<CSL> m_cpuLocal;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_gpuLocal;
    GeometryConstsCpu g_cpu;
    Microsoft::WRL::ComPtr<ID3D11Buffer> g_gpu;

    float m_radius = 1.0f / 10.0f;
    StructuredBuffer<Particle> particles;
    int m_size;

  private:
    float rho_0 = 1.f;
    float k = 1.f;
    float friction = 0.1f;
};
} // namespace soku
