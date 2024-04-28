#include "pch.h"
#include "Texture2D.h"
#include "Utils.h"
namespace soku {
void Texture2D::Initialize(const int &width, const int &height,
                           const DXGI_FORMAT &format,
                           Microsoft::WRL::ComPtr<ID3D11Device> &device) {
   
   D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 1;
    txtDesc.ArraySize = 1;
    txtDesc.Format = format; // 주로 FLOAT 사용
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET |
                        D3D11_BIND_UNORDERED_ACCESS;
    txtDesc.MiscFlags = 0;
    txtDesc.CPUAccessFlags = 0;

    ThrowIfFailed(
        device->CreateTexture2D(&txtDesc, NULL, m_tex.GetAddressOf()));
    ThrowIfFailed(device->CreateRenderTargetView(m_tex.Get(), NULL,
                                                 m_rtv.GetAddressOf()));
    ThrowIfFailed(device->CreateShaderResourceView(m_tex.Get(), NULL,
                                                   m_srv.GetAddressOf()));
    ThrowIfFailed(device->CreateUnorderedAccessView(m_tex.Get(), NULL,
                                                    m_uav.GetAddressOf()));
}
ID3D11RenderTargetView *Texture2D::GetRTV() { return m_rtv.Get(); }
ID3D11RenderTargetView **Texture2D::GetAddressOfRTV() {
    return m_rtv.GetAddressOf();
}
ID3D11Texture2D *Texture2D::GetTex() { return m_tex.Get(); }
} // namespace soku
