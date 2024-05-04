#include "pch.h"
#include "Sph.h"
#include "GraphicsCommon.h"
#include "Utils.h"
namespace soku {
void Sph::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                     const int &particleCounts) {
    particles.m_cpu.resize(particleCounts);
    m_size = particleCounts;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomPosition(-0.8f, 0.8f);
    for (auto &p : particles.m_cpu) {
        p.position = Vector3(randomPosition(gen), randomPosition(gen), 0.f);
        p.time = 1.f;
        p.width = m_radius;
        p.color = Vector3(0.f, 0.f, 1.f);
    }
    particles.Initilaize(device);
    Utils::CreateConstantBuffer(g_cpu, g_gpu, device);
}
void Sph::Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                 float &dt, const float ratio) {
    dt = 1.f / 240.f;

    UpdateDensity();
    UpdateAcceleration();
    Vector3 g = Vector3(0.f, -9.8f, 0.f);
    for (auto &p : particles.m_cpu) {
        p.velocity += (p.acceleration + g) * dt;
        p.position += p.velocity * dt;
    }
    const float cor = 0.5f;
    const float ground = -0.8f;
    const float lWall = -0.8f;
    const float rWall = 0.8f;

    for (auto &p : particles.m_cpu) {
        if (p.position.y < ground && p.velocity.y < 0.0f) {
            p.velocity.y *= -cor;
            p.position.y = ground;
        }
        if (p.position.x < lWall && p.velocity.x < 0.0f) {
            p.velocity.x *= -cor;
            p.position.x = lWall;
        }
        if (p.position.x > rWall && p.velocity.x > 0.0f) {
            p.velocity.x *= -cor;
            p.position.x = rWall;
        }
    }
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(particles.GetStaging(), 0, D3D11_MAP_WRITE, 0, &ms);
    memcpy(ms.pData, particles.m_cpu.data(),
           sizeof(Particle) * particles.m_cpu.size());
    context->Unmap(particles.GetStaging(), 0);
    context->CopyResource(particles.GetGpu(), particles.GetStaging());
    g_cpu.ratio = ratio;
    Utils::UpdateConstantBuffer(g_cpu, g_gpu, context);
}
void Sph::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Graphics::drawingParticlesBlendPSO.SetPipelineState(context);
    context->CSSetShader(NULL, NULL, 0);
    context->VSSetShaderResources(0, 1, particles.GetSrvAddressOf());
    context->GSSetConstantBuffers(0, 1, g_gpu.GetAddressOf());
    context->Draw((UINT)particles.m_cpu.size(), 0);
    
    ID3D11ShaderResourceView *nullSRVs[1] = {0};
    context->VSSetShaderResources(0, 1, nullSRVs);
}
void Sph::UpdateDensity() {

#pragma omp parallel for
    for (int i = 0; i < m_size; i++) {
        particles.m_cpu[i].density = 0.f;
        auto &p_i = particles.m_cpu[i];
        
        for (int j = 0; j < m_size; j++) {
            auto &p_j = particles.m_cpu[j];   
            double dist = (p_i.position - p_j.position).Length();
            
            if (dist >= m_radius)
                continue;

            float w = CubicSpline(2.f * dist / m_radius);
            p_i.density += p_j.mass * w;
        }
        p_i.pressure = k * (pow(p_i.density / rho_0, 7) - 1.f);
    }
}
void Sph::UpdateAcceleration() {

#pragma omp parallel for
    for (int i = 0; i < m_size; i++) {
        particles.m_cpu[i].acceleration = Vector3::Zero;
        auto &p_i = particles.m_cpu[i];

        for (int j = 0; j < m_size; j++) {
            auto &p_j = particles.m_cpu[j]; 
            
            float &pr_i = p_i.pressure;
            float &pr_j = p_j.pressure;
            float &rho_i = p_i.density;
            float &rho_j = p_j.density;

            Vector3 &x_i = p_i.position;
            Vector3 &x_j = p_j.position;
            Vector3 x_ij = x_i - x_j;
            Vector3 &v_i = p_i.velocity;
            Vector3 &v_j = p_j.velocity;
            Vector3 v_ij = v_i - v_j;
            double dist = (x_ij).Length();

            if (dist > m_radius || i == j || dist < 1e-3)
                continue;
            Vector3 delW =
                (x_ij / dist) * CubicSplineGrad(2.f * dist / m_radius);

            Vector3 pressureAcc =
                (pr_i / (rho_i * rho_i) +pr_j / (rho_j * rho_j)) * delW;
            Vector3 viscosifyAcc =
                (2.f / rho_j) * ((x_ij.Dot(delW)) /
                       (0.01f * m_radius * m_radius + x_ij.Dot(x_ij)))*v_ij;

            p_i.acceleration += -pressureAcc + friction * viscosifyAcc;
        }
    }
}
float Sph::CubicSplineGrad(const float q) {
    constexpr float coeff = 3.0f / (2.0f * 3.141592f);

    if (q < 1.0f)
        return coeff * (-2.0f * q + 1.5f * q * q);
    else if (q < 2.0f)
        return coeff * -0.5f * (2.0f - q) * (2.0f - q);
    else // q >= 2.0f
        return 0.0f;
}
float Sph::CubicSpline(const float q) {
    constexpr float coeff = 3.0f / (2.0f * 3.141592f);

    if (q < 1.0f)
        return coeff * (2.0f / 3.0f - q * q + 0.5f * q * q * q);
    else if (q < 2.0f)
        return coeff * pow(2.0f - q, 3.0f) / 6.0f;
    else // q >= 2.0f
        return 0.0f;
}
} // namespace soku
