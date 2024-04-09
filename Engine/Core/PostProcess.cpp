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
      
    
    // Create Filter Mesh
    auto screen = GeometryGenerator::MakeSquare();
    m_meshes = std::make_shared<Mesh>();
    Utils::CreateVertexBuffer(screen.m_vertices, m_meshes->m_vertexBuffer,
                              device);
    Utils::CreateIndexBuffer(screen.m_indices, m_meshes->m_indexBuffer, device);
    m_meshes->m_indexCount = screen.m_indices.size();    
    
    // Create Filters
    for (int i = 0; i < 5; i++) {
        int scailIdx = pow(2, i);
        ImageFilter downFilter(width / scailIdx, height / scailIdx, device);
        downFilters.push_back(downFilter);
    }
    for (int i = 4; i >= 0; i--) {
        int scailIdx = pow(2, i);
        ImageFilter upFilter(width / scailIdx, height / scailIdx, device);
        upFilters.push_back(upFilter);
    }
    combineFilter.Initialize(width, height, device);
    combineFilter.SetRenderTargetViews(targets);
    combineFilter.SetShaderResourceViews(resources);
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
    
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_meshes->m_vertexBuffer.GetAddressOf(),
                                &stride, &offset);
    context->IASetIndexBuffer(m_meshes->m_indexBuffer.Get(),
                              DXGI_FORMAT_R32_UINT, 0);

   /* Graphics::downSamplingPSO.SetPipelineState(context);
    for (int i = 0; i < downFilters.size(); i++) {
        downFilters[i].Render(context);
        context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
    }
    for (int i = 0; i < upFilters.size(); i++) {
        upFilters[i].Render(context);
        context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
    }*/
    
    combineFilter.Render(context);
    context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
}
} // namespace soku
