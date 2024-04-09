#pragma once
#include "Constants.h"
namespace soku {
class ImageFilter {
  public:
    ImageFilter() {}
    ImageFilter(const int &width, const int &height, 
        Microsoft::WRL::ComPtr<ID3D11Device>& device);
    void Initialize(const int &width, const int &height,
                    Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void SetRenderTargetViews(
        const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>
            &rtvs);
    void SetShaderResourceViews(
        const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>
            &srvs);
    D3D11_VIEWPORT m_viewport;
    SamplingPSConstants samplingConstantCPU;
    Microsoft::WRL::ComPtr<ID3D11Buffer> samplingConstantGPU;
  public:
    std::vector<ID3D11ShaderResourceView *> m_srvs;
    std::vector<ID3D11RenderTargetView *> m_rtvs;
};
} // namespace soku