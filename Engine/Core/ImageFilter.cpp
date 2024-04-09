#include "pch.h"
#include "ImageFilter.h"
#include "Utils.h"
namespace soku {
ImageFilter::ImageFilter(const int &width, const int &height, 
                         Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    this->Initialize(width, height, device);
}
void ImageFilter::Initialize(const int &width, const int &height,
                             Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    m_viewport = {0.f, 0.f, (FLOAT)width, (FLOAT)height, 0.f, 1.f};
    samplingConstantCPU.dx = 1.f / width;
    samplingConstantCPU.dy = 1.f / height;
    Utils::CreateConstantBuffer(&samplingConstantCPU, samplingConstantGPU,
                                device);
}
void ImageFilter::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    
    context->RSSetViewports(1, &m_viewport);
    context->OMSetRenderTargets(m_rtvs.size(), m_rtvs.data(), NULL);
    context->PSSetConstantBuffers(0, 1, samplingConstantGPU.GetAddressOf());
    //context->PSSetShader(m_pixelShader.Get(), NULL, 0);
    context->PSSetShaderResources(0, m_srvs.size(), m_srvs.data());
}

void ImageFilter::SetRenderTargetViews(
    const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &rtvs) {
    m_rtvs.clear();
    for (const auto &rtv : rtvs) {
        m_rtvs.push_back(rtv.Get());
    }
}
void ImageFilter::SetShaderResourceViews(
    const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &srvs) {
    m_srvs.clear();
    for (const auto &srv : srvs) {
        m_srvs.push_back(srv.Get());
    }
}

} // namespace soku
