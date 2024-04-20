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
    const int &height,
    const int & bloomLevel) {
      
    
    // Create Filter Mesh
    auto screen = GeometryGenerator::MakeSquare();
    m_meshes = std::make_shared<Mesh>();
    Utils::CreateVertexBuffer(screen.m_vertices, m_meshes->m_vertexBuffer,
                              device);
    Utils::CreateIndexBuffer(screen.m_indices, m_meshes->m_indexBuffer, device);
    m_meshes->m_indexCount = (UINT)screen.m_indices.size();    
    SRVs.resize(bloomLevel + 1);
    RTVs.resize(bloomLevel + 1);

    // Create Filters
    for (int i = 0; i < bloomLevel+1; i++) {
        int scale = (int)pow(2, i);
        CreateBuffer(width / scale, height / scale, SRVs[i], RTVs[i],
                     device);
    }
    CreateBuffer(width , height , SRVs[0], RTVs[0], device);
    for (int i = 0; i < bloomLevel; i++) {
        int scale = (int)pow(2, i + 1);
        ImageFilter downFilter(width / scale, height / scale,
                               device);
        downFilter.SetRenderTargetViews({RTVs[i+1]});
        if (i == 0) {
            downFilter.SetShaderResourceViews(resources);
        } else {
            downFilter.SetShaderResourceViews({SRVs[i]});     
        }
        downFilters.push_back(downFilter);
    }
    for (int i = bloomLevel-1; i >= 0; i--) {
        int scale = (int)pow(2, i);
        ImageFilter upFilter(width / scale, height / scale, device);
        upFilter.SetShaderResourceViews({SRVs[i + 1]});
        upFilter.SetRenderTargetViews({RTVs[i]});
        upFilters.push_back(upFilter);
    }
    combineFilter.Initialize(width, height, device);
    combineFilter.SetShaderResourceViews({resources[0], SRVs[0]});
    combineFilter.SetRenderTargetViews(targets);
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

   Graphics::downSamplingPSO.SetPipelineState(context);
    for (int i = 0; i < downFilters.size(); i++) {
        downFilters[i].Render(context);
        context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
        Utils::SRVBarrier(context);
    }
    Graphics::upSamplingPSO.SetPipelineState(context);
    for (int i = 0; i < upFilters.size(); i++) {
        upFilters[i].Render(context);
        context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
        Utils::SRVBarrier(context);
    }
    Graphics::combinePSO.SetPipelineState(context);
    combineFilter.Render(context);
    context->DrawIndexed(m_meshes->m_indexCount, 0, 0);
    Utils::SRVBarrier(context);
}
void PostProcess::Update(const SamplingPSConstants &constant,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    combineFilter.samplingConstantCPU.bloomStrength = constant.bloomStrength;
    combineFilter.samplingConstantCPU.expose = constant.expose;
    combineFilter.samplingConstantCPU.gamma = constant.gamma;
    Utils::UpdateConstantBuffer(combineFilter.samplingConstantCPU,
                                combineFilter.samplingConstantGPU, context);
}
} // namespace soku
