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
Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyboxRS;

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskingDSS;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskedDrawDSS;


Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
Microsoft::WRL::ComPtr<ID3D11InputLayout> combineIL;

Microsoft::WRL::ComPtr<ID3D11BlendState> basicBS;

GraphicsPSO defaultSolidPSO;
GraphicsPSO skyboxPSO;
GraphicsPSO combinePSO;
GraphicsPSO upSamplingPSO;
GraphicsPSO downSamplingPSO;
GraphicsPSO mirrorMaskingPSO;
GraphicsPSO mirrorPSO;
GraphicsPSO blendPSO;
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
    device->CreateRasterizerState(&rasterDesc, skyboxRS.GetAddressOf());

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false;
    device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf());

    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    device->CreateDepthStencilState(&dsDesc, maskingDSS.GetAddressOf());

    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    device->CreateDepthStencilState(&dsDesc, maskingDSS.GetAddressOf());

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
    device->CreateInputLayout(basicIEs.data(), (UINT)basicIEs.size(), g_pDefaultVS,
                              sizeof(g_pDefaultVS), basicIL.GetAddressOf());

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.RenderTarget[0].BlendEnable = true;

    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha =
        D3D11_BLEND::D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
        device->CreateBlendState(&blendDesc, basicBS.GetAddressOf());

    defaultSolidPSO.SetVertexShader(g_pDefaultVS, sizeof(g_pDefaultVS), device);
    defaultSolidPSO.SetPixelShader(g_pDefaultPS, sizeof(g_pDefaultPS), device);
    defaultSolidPSO.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    defaultSolidPSO.SetSamplerState(linearWrapSS);
    defaultSolidPSO.SetRasterizerState(solidRS);
    defaultSolidPSO.SetDepthStencilState(drawDSS);
    defaultSolidPSO.SetInputLayout(basicIL);

    mirrorMaskingPSO = defaultSolidPSO;
    mirrorMaskingPSO.SetDepthStencilState(maskingDSS);
    mirrorPSO = defaultSolidPSO;
    mirrorPSO.SetDepthStencilState(maskedDrawDSS);
    skyboxPSO = defaultSolidPSO;
    skyboxPSO.SetVertexShader(g_pSkyboxVS, sizeof(g_pSkyboxVS), device);
    skyboxPSO.SetPixelShader(g_pSkyboxPS, sizeof(g_pSkyboxPS), device);
    skyboxPSO.SetRasterizerState(skyboxRS);

    combinePSO = defaultSolidPSO;
    combinePSO.SetSamplerState(linearClampSS);
    combinePSO.SetVertexShader(g_pCombineVS, sizeof(g_pCombineVS), device);
    combinePSO.SetPixelShader(g_pCombinePS, sizeof(g_pCombinePS), device);

    upSamplingPSO = combinePSO;
    upSamplingPSO.SetPixelShader(g_pUpSamplingPS, sizeof(g_pUpSamplingPS),
                                 device);
    
    downSamplingPSO = combinePSO;
    downSamplingPSO.SetPixelShader(g_pDownSamplingPS, sizeof(g_pDownSamplingPS),
                                 device);

    blendPSO = defaultSolidPSO;
    blendPSO.SetBlendState(basicBS);

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
