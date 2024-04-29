#include "pch.h"
#include "Texture2D.h"
#include "Utils.h"
namespace soku {
void Texture2D::Initialize(const int &width, const int &height,
                           const DXGI_FORMAT &format,
                           Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    Utils::CreateUATexture(width, height, format, device,m_tex,m_srv,m_uav,m_rtv);
}
} // namespace soku