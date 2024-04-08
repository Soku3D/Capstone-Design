#pragma once

namespace soku {
class ImageFilter {
  public:
    ImageFilter() {}
    ImageFilter(const int &width, const int &height);
    void Initialize(const int &width, const int &height);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void SetRenderTargetViews(
        const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>
            &rtvs);
    void SetShaderResourceViews(
        const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>
            &srvs);
    D3D11_VIEWPORT m_viewport;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
  public:
    std::vector<ID3D11ShaderResourceView *> m_srvs;
    std::vector<ID3D11RenderTargetView *> m_rtvs;
};
} // namespace soku