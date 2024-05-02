#pragma once
#include "StructuredBuffer.h"

namespace soku {
class Sph {
  public:
    struct Particle {
        Vector3 position;
        Vector3 color;
        Vector3 velocity;
        Vector3 acceleration;
        float width;
        float time;
        float mass;
        float density;
        float pressure;
    };
    struct CSConsts {
        float dt;
        Vector3 dummy;
    };
    Sph(){}
    // Initialize Particles Position, Velocity, Color, etc.
    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    const int &particleCounts);
    // Update Particles
    void Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                float &dt);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    float CubicSpline(const float &q);
    float CubicSplineGrad(const float &q);
    CSConsts m_cpuConsts;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_gpuConsts;
    float m_radius = 1.0f / 16.0f;
    StructuredBuffer<Particle> particles;
};
}

