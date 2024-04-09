#include "pch.h"
#include "GraphicsCommon.h"

#include "CompiledShaders/SkyboxVS.h"
#include "CompiledShaders/SkyboxPS.h"
#include "CompiledShaders/DefaultVS.h"
#include "CompiledShaders/DefaultPS.h"
#include "CompiledShaders/CombineVS.h"
#include "CompiledShaders/CombinePS.h"
#include "CompiledShaders/UpSamplingPS.h"
#include "CompiledShaders/DownSamplingPS.h"




namespace soku {
namespace Graphics {
Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;

Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRS;
Microsoft::WRL::ComPtr<ID3D11RasterizerState> cubeRS;

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS;

Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
Microsoft::WRL::ComPtr<ID3D11InputLayout> combineIL;

GraphicsPSO defaultSolidPSO;
GraphicsPSO skyboxPSO;
GraphicsPSO combinePSO;
GraphicsPSO upSamplingPSO;
GraphicsPSO downSamplingPSO;

void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    // Create SamplerState
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&samplerDesc, linearWrapSS.GetAddressOf());

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&samplerDesc, linearClampSS.GetAddressOf());

    // Create RasterizerState;
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false;
    // rasterDesc.ScissorEnable = true;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.MultisampleEnable = true;
    device->CreateRasterizerState(&rasterDesc, solidRS.GetAddressOf());
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = true;
    device->CreateRasterizerState(&rasterDesc, cubeRS.GetAddressOf());

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false;
    device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf());

    std::vector<D3D11_INPUT_ELEMENT_DESC> basicIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0}};
    std::vector<D3D11_INPUT_ELEMENT_DESC> IEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    device->CreateInputLayout(basicIEs.data(), basicIEs.size(), g_pDefaultVS,
                              sizeof(g_pDefaultVS), basicIL.GetAddressOf());

  
    defaultSolidPSO.SetVertexShader(g_pDefaultVS, sizeof(g_pDefaultVS), device);
    defaultSolidPSO.SetPixelShader(g_pDefaultPS, sizeof(g_pDefaultPS), device);
    defaultSolidPSO.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    defaultSolidPSO.SetSamplerState(linearWrapSS);
    defaultSolidPSO.SetRasterizerState(solidRS);
    defaultSolidPSO.SetDepthStencilState(drawDSS);
    defaultSolidPSO.SetInputLayout(basicIL);

    skyboxPSO = defaultSolidPSO;
    skyboxPSO.SetVertexShader(g_pSkyboxVS, sizeof(g_pSkyboxVS), device);
    skyboxPSO.SetPixelShader(g_pSkyboxPS, sizeof(g_pSkyboxPS), device);
    skyboxPSO.SetRasterizerState(cubeRS);

    combinePSO = defaultSolidPSO;
    
    combinePSO.SetVertexShader(g_pCombineVS, sizeof(g_pCombineVS), device);
    combinePSO.SetPixelShader(g_pCombinePS, sizeof(g_pCombinePS), device);

    upSamplingPSO = combinePSO;
    upSamplingPSO.SetPixelShader(g_pUpSamplingPS, sizeof(g_pUpSamplingPS),
                                 device);
    
    downSamplingPSO = combinePSO;
    downSamplingPSO.SetPixelShader(g_pDownSamplingPS, sizeof(g_pDownSamplingPS),
                                 device);

}


/*void InitSamplers(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitRasterizerStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitBlendStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitDepthStencilStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitPipelineStates(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}
void InitShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
}*/
} // namespace Graphics
} // namespace soku
