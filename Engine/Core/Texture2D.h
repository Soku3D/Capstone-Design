#pragma once

namespace soku {
class Texture2D {
  public:
    void Initialize(const int &width, const int &height,
                    const DXGI_FORMAT &format,
                    Microsoft::WRL::ComPtr<ID3D11Device> &device);
    ID3D11RenderTargetView *GetRTV();
    ID3D11RenderTargetView **GetAddressOfRTV();
    ID3D11Texture2D *GetTex();
    ID3D11UnorderedAccessView *GetUAV() { return m_uav.Get(); }
  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tex;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;
};
} // namespace soku