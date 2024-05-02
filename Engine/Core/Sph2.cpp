#include "pch.h"
#include "Sph.h"
#include "GraphicsCommon.h"
#include "Utils.h"
namespace soku {
void Sph::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                     const int &particleCounts) {
    particles.m_cpu.resize(particleCounts);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomV(0.f, 1.f);
    std::uniform_real_distribution<float> randomW(0.01f, 0.05f);
    std::uniform_real_distribution<float> randomR(0, 0.1f);
    std::uniform_real_distribution<float> randomC(0.f, 1.f);
    std::uniform_real_distribution<float> randomT(-3.14f, 3.14f);
    std::uniform_real_distribution<float> randomL(-5.f, 0.3f);
    for (auto &p : particles.m_cpu) {
        float theta = randomT(gen);
        p.position = Vector3::Zero;
        p.velocity = Vector3(-1.f, 0.f, 0.f);
        p.color = Vector3(randomC(gen), randomC(gen), randomC(gen));
        p.width = m_radius;
        p.time = -1.f;
        // p.time = randomL(gen);
        p.acceleration = Vector3::Zero;
        p.mass = 1.f;
    }
    particles.Initilaize(device);
    Utils::CreateConstantBuffer(m_cpuConsts, m_gpuConsts, device);
}
void Sph::Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                 float &dt) {
    dt = 1.0f / 60.0f * 0.25f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
    std::uniform_real_distribution<float> randomSpeed(1.5f, 2.0f);
    std::uniform_real_distribution<float> randomLife(0.0f, 1.0f);
    int count = 1;
    for (auto &p : particles.m_cpu) {
        if (p.time < 0 && count > 0) {
            const float theta = randomTheta(gen);

            p.position =
                Vector3(cos(theta), -sin(theta), 0.0) * randomLife(gen) * 0.1f +
                Vector3(-0.5f, 0.5f, 0.0f);
            p.velocity = Vector3(1.0f, 0.0f, 0.0);
            p.time = randomLife(gen) * 1.5f;
            p.width = m_radius;
            count--;
        }
    }

#pragma omp parallel for
    for (int i = 0; i < particles.m_cpu.size(); i++) {
        
        

        auto &p_i = particles.m_cpu[i];
        if (p_i.time < 0.f)
            continue;
        p_i.dencity = 0.f;
        p_i.pressure = 0.f;
        float lo_0 = 1.f;
        float k = 1.f;
        for (int j = 0; j < particles.m_cpu.size(); j++) {

            const auto &p_j = particles.m_cpu[j];
            if (p_j.time < 0.f)
                continue;
            float dist = (p_i.position - p_j.position).Length();
            if (dist > m_radius)
                continue;
         
            float w = GetWeight((2 * dist) / m_radius);
            p_i.dencity += p_j.mass * w;
        }
        p_i.pressure = k * (std::pow(p_i.density / lo_0, 7.f) - 1.f);
    }
#pragma omp parallel for
    for (int i = 0; i < particles.m_cpu.size(); i++) {
        auto &p_i = particles.m_cpu[i];
        Vector3 pressure = Vector3::Zero;
        Vector3 viscosity = Vector3::Zero;
        Vector3 g = Vector3(0.f, -9.8f, 0.f);
        float& pr_i = p_i.pressure;
        float& lo_i = p_i.density;
        float& nu = 0.1f;
        if (p_i.time < 0.f)
            continue;
        for (int j = 0; j < particles.m_cpu.size(); j++) {
            auto &p_j = particles.m_cpu[j];
             if (p_j.time < 0.0f)
                continue;
                    if(i==j)
                    continue;
            Vector3 x_i = p_i.position;
            Vector3 x_j = p_j.position;
            Vector3 x_ij = x_i - x_j;
            Vector3 v_i = p_i.velocity;
            Vector3 v_j = p_j.velocity;
            Vector3 v_ij = v_i - v_j;
            float lo_j = p_j.density;
            float pr_j = p_j.pressure;

            float dist = (p_i.position - p_j.position).Length();
            if (dist > m_radius)
                continue;
            if (dist <= 1e-3)
                continue;
            float delW = GetDelWeight((2 * dist) / m_radius);
            pressure += x_ij/dist *
                        (lo_i * p_j.mass *
                         (pr_i / lo_i * lo_i + pr_j / lo_j * lo_j) * delW);
            viscosity += 2.f * (p_j.mass / lo_j) * v_ij *
                (delW / (x_ij.Dot(x_ij) + (0.01f * m_radius * m_radius)));
        }
        pressure /= -lo_i;
        viscosity *= nu;
        p_i.acceleration = pressure + viscosity;
    }
    for (int i = 0; i < particles.m_cpu.size(); i++) {

        if (particles.m_cpu[i].time < 0.0f)
            continue;

        particles.m_cpu[i].velocity +=
            particles.m_cpu[i].acceleration * dt / 1.f;
        particles.m_cpu[i].position += particles.m_cpu[i].velocity * dt;
    }
    
    float cof = 0.5f;
    static const Vector3 g = Vector3(0.f, -9.8f, 0.f);
    for (auto &p : particles.m_cpu) {
        if (p.time < 0)
            continue;
        p.velocity += g * dt;
        // p.position += p.velocity * dt;
        if (p.position.y < -0.8f && p.velocity.y < 0.f) {
            p.position.y = -0.8f;
            p.velocity.y *= -cof;
        }
        if (p.position.y >= 1.f && p.velocity.y > 0.f) {
            p.position.y = 1.f;
            p.velocity.y *= -cof;
        }
        if (p.position.x <= -1.f && p.velocity.x < 0.f) {
            p.position.x = -1.f;
            p.velocity.x *= -cof;
        }
        if (p.position.x >= 1.f && p.velocity.x > 0.f) {
            p.position.x = 1.f;
            p.velocity.x *= -cof;
        }
    }
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(particles.GetStaging(), 0, D3D11_MAP_WRITE, 0, &ms);
    memcpy(ms.pData, particles.m_cpu.data(),
           sizeof(Particle) * particles.m_cpu.size());
    context->Unmap(particles.GetStaging(), 0);

    context->CopyResource(particles.GetGpu(), particles.GetStaging());
    /*  m_cpuConsts.dt = dt;
      Utils::UpdateConstantBuffer(m_cpuConsts, m_gpuConsts, context);

       Graphics::updateParticlePSO.SetPipelineState(context);
       context->CSSetConstantBuffers(0, 1, m_gpuConsts.GetAddressOf());
       context->CSSetUnorderedAccessViews(0, 1, particles.GetUavAddressOf(),
      NULL); context->Dispatch((UINT)std::ceil(particles.m_cpu.size()/256.f), 1,
      1); Utils::ComputeShaderBarrier(context);*/
}
void Sph::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Graphics::drawingParticlesBlendPSO.SetPipelineState(context);
    context->CSSetShader(NULL, NULL, 0);
    context->VSSetShaderResources(0, 1, particles.GetSrvAddressOf());
    context->Draw((UINT)particles.m_cpu.size(), 0);
    ID3D11ShaderResourceView *nullSRVs[1] = {0};
    context->VSSetShaderResources(0, 1, nullSRVs);
}
float Sph::CubicSpline(const float &q) {
    float ret = 0.f;
    if (q >= 2.f)
        return ret;
    else if (q >= 1) {
        ret =  std::pow(2.f - q, 3.f) / 6.f;
    } else {
        ret =  2.f / 3.f - q * q + 0.5f * q * q * q;
    }
    return (ret * 1.5f) / 3.141592f;
}
float Sph::CubicSplineGrad(const float &q) {
    float ret = 0.f;
    if (q >= 2.f)
        return 0;
    else if (q >= 1) {
        ret = -std::pow(2.f - q, 3.f) / 2.f;
    } else {
        ret = -2 * q + 3.f * 0.5f * q * q;
    }
    return (ret * 1.5f) / 3.141592f;
}
} // namespace soku
