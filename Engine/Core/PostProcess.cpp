#include "pch.h"
#include "PostProcess.h"
#include "Utils.h"
#include "GraphicsCommon.h"
#include "GeometryGenerator.h"
namespace soku {
void PostProcess::Initialize(
    Microsoft::WRL::ComPtr<ID3D11Device> &device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
    const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>  &resources,
    const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &targets,
    const int &width,
    const int &height) {
    combineFilter.Initialize(width, height);
    combineFilter.SetRenderTargetViews(targets);
    combineFilter.SetShaderResourceViews(resources);
    auto screen = GeometryGenerator::MakeSquare();
    model = std::make_shared<Model>();
    model->Initialize({screen}, device);

}
void PostProcess::CreateBuffer(
    const int &width, const int &height,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &outputSRV,
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &outputRTV,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.MipLevels = texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D> buff;
    device->CreateTexture2D(&texDesc, NULL, buff.GetAddressOf());

    ThrowIfFailed(device->CreateRenderTargetView(buff.Get(), NULL,
                                                 outputRTV.GetAddressOf()));
    ThrowIfFailed(device->CreateShaderResourceView(buff.Get(), NULL,
                                                 outputSRV.GetAddressOf()));
}
void PostProcess::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    Graphics::combinePSO.SetPipelineState(context);
    combineFilter.Render(context);
    model->Render(context);
}
} // namespace soku
