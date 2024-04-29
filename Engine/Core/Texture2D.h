#pragma once

namespace soku {
class Texture2D {
  public:
    void Initialize(const int &width, const int &height,
                    const DXGI_FORMAT &format,
                    Microsoft::WRL::ComPtr<ID3D11Device> &device);

    ID3D11Texture2D *GetTex() { return m_tex.Get(); }
    ID3D11RenderTargetView *GetRtv() { return m_rtv.Get(); }
    ID3D11RenderTargetView **GetRtvAddressOf() { return m_rtv.GetAddressOf(); }
    ID3D11ShaderResourceView **GetSrvAddressOf() { return m_srv.GetAddressOf(); }
    ID3D11UnorderedAccessView **GetUavAddressOf() { return m_uav.GetAddressOf(); }

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tex;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;
};
} // namespace soku