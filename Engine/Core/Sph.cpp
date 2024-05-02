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
        p.position = Vector3::Zero +
                     Vector3(std::cos(theta), sin(theta), 0.f) * randomR(gen);
        p.velocity = Vector3(-1.f, 0.f, 0.f) * randomV(gen);
        p.color = Vector3(randomC(gen), randomC(gen), randomC(gen));
        p.width = randomW(gen);
        p.time = -1.f;
        //  p.time = randomL(gen);
        p.acceleration = Vector3(0.f, -9.8f, 0.f);
    }
    particles.Initilaize(device);
    Utils::CreateConstantBuffer(m_cpuConsts, m_gpuConsts, device);
}
void Sph::Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                 float &dt) {
    int count = 10;
    for (auto &p : particles.m_cpu) {
        if (p.time < 0 && count > 0) {
            p.time = 1.f;
            count--;
        }
    }
    float radius = 0.3f;
    for (int i = 0; i < particles.m_cpu.size(); i++) {
        float lo_i = 0;
        float mass = 1.f;
        for (int j = 0; j < particles.m_cpu.size(); j++) {
            auto &p_i = particles.m_cpu[i];
            const auto &p_j = particles.m_cpu[j];

            float dist = (p_i.position - p_j.position).Length();
            if (dist > radius)
                continue;
            if (dist <= 1e-3)
                continue;
            float w = GetWeight((2 * dist) / radius);
            lo_i += mass * w;
        }
    }
 /*   for (auto &p : particles.m_cpu) {
        if (p.time < 0)
            continue;
        Vector3 g = Vector3(0.f, -9.8f, 0.f);
        p.velocity += g * dt;
        p.position += p.velocity * dt;
    }*/
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
    context->CSSetUnorderedAccessViews(0, 1, particles.GetUavAddressOf(), NULL);
    context->Dispatch((UINT)std::ceil(particles.m_cpu.size()/256.f), 1, 1);
    Utils::ComputeShaderBarrier(context);*/

}
void Sph::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Graphics::drawingParticlesBlendPSO.SetPipelineState(context);
    context->CSSetShader(NULL, NULL, 0);
    context->VSSetShaderResources(0, 1, particles.GetSrvAddressOf());
    context->Draw((UINT)particles.m_cpu.size(), 0);
    ID3D11ShaderResourceView *nullSRVs[1] = {0};
    context->VSSetShaderResources(0, 1, nullSRVs);
}
} // namespace soku
