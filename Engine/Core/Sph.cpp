#include "pch.h"
#include "Sph.h"
#include "GraphicsCommon.h"
#include "Utils.h"
namespace soku {
void Sph::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                     const int &particleCounts) {
    particles.m_cpu.resize(particleCounts);

    particles.Initilaize(device);
    Utils::CreateConstantBuffer(m_cpuConsts, m_gpuConsts, device);
}
void Sph::Update(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                 float &dt) {

    m_cpuConsts.dt = dt;
    Utils::UpdateConstantBuffer(m_cpuConsts, m_gpuConsts, context);

    Graphics::updateParticlePSO.SetPipelineState(context);
    context->CSSetConstantBuffers(0, 1, m_gpuConsts.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, particles.GetUavAddressOf(), NULL);
    context->Dispatch((UINT)std::ceil(particles.m_cpu.size() / 256.f), 1, 1);
    Utils::ComputeShaderBarrier(context);
}
void Sph::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Graphics::drawingParticlesBlendPSO.SetPipelineState(context);
    context->CSSetShader(NULL, NULL, 0);
    context->VSSetShaderResources(0, 1, particles.GetSrvAddressOf());
    context->Draw((UINT)particles.m_cpu.size(), 0);
    ID3D11ShaderResourceView *nullSRVs[1] = {0};
    context->VSSetShaderResources(0, 1, nullSRVs);
}
float Sph::CubicSpline(const float &q) {}
float Sph::CubicSplineGrad(const float &q) {}
} // namespace soku
