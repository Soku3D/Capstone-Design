#include "pch.h"
#include "GraphicsPSO.h"
#include "Utils.h"

namespace soku {
void GraphicsPSO::operator=(const GraphicsPSO &pso) {
    this->m_PSODesc = pso.m_PSODesc;
}
void GraphicsPSO::SetPipelineState(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    context->IASetPrimitiveTopology(m_PSODesc.m_topology);
    context->IASetInputLayout(m_PSODesc.m_inputLayout.Get());

    context->VSSetShader(m_PSODesc.m_vertexShader.Get(), NULL, 0);
    context->PSSetShader(m_PSODesc.m_pixelShader.Get(), NULL, 0);
    context->DSSetShader(m_PSODesc.m_domainShader.Get(), NULL, 0);
    context->HSSetShader(m_PSODesc.m_hullShader.Get(), NULL, 0);
    context->GSSetShader(m_PSODesc.m_geometryShader.Get(), NULL, 0);

    context->RSSetState(m_PSODesc.m_rasterizerState.Get());
    context->OMSetDepthStencilState(m_PSODesc.m_depthStencilState.Get(), 1.f);
    context->VSSetSamplers(0, 1, m_PSODesc.m_samplerState.GetAddressOf());
    context->PSSetSamplers(0, 1, m_PSODesc.m_samplerState.GetAddressOf());
}
void GraphicsPSO::SetSamplerState(
    Microsoft::WRL::ComPtr<ID3D11SamplerState> &samplerState) {
    m_PSODesc.m_samplerState = samplerState;
}
void GraphicsPSO::SetRasterizerState(
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> &rasterizerState) {
    m_PSODesc.m_rasterizerState = rasterizerState;
}
void GraphicsPSO::SetInputLayout(
    Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout) {
    m_PSODesc.m_inputLayout = inputLayout;
}
void GraphicsPSO::SetDepthStencilState(
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> &depthStencilState) {
    m_PSODesc.m_depthStencilState = depthStencilState;
}
void GraphicsPSO::SetBlendState(
    Microsoft::WRL::ComPtr<ID3D11BlendState> &blendState) {
    m_PSODesc.m_blendState = blendState;
}
void GraphicsPSO::SetPrimitiveTopology(
    const D3D11_PRIMITIVE_TOPOLOGY &topology) {
    m_PSODesc.m_topology = topology;
}
void GraphicsPSO::SetVertexShader(const void *Binary, size_t Size,
                                  Microsoft::WRL::ComPtr<ID3D11Device> &device){
    ThrowIfFailed(device->CreateVertexShader(
        Binary, Size, nullptr, m_PSODesc.m_vertexShader.GetAddressOf()));
}
void GraphicsPSO::SetPixelShader(const void *Binary, size_t Size,
                                 Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    ThrowIfFailed(device->CreatePixelShader(
        Binary, Size, nullptr, m_PSODesc.m_pixelShader.GetAddressOf()));
}
void GraphicsPSO::SetGeometryShader(const void *Binary, size_t Size,
                                 Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    ThrowIfFailed(device->CreateGeometryShader(
        Binary, Size, nullptr, m_PSODesc.m_geometryShader.GetAddressOf()));
}
void ComputePSO::operator=(const ComputePSO &pso) {
    this->m_PSODesc = pso.m_PSODesc;
}
void ComputePSO::SetPipelineState(
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    
    context->VSSetShader(NULL, NULL, 0);
    context->PSSetShader(NULL, NULL, 0);
    context->DSSetShader(NULL, NULL, 0);
    context->HSSetShader(NULL, NULL, 0);
    context->GSSetShader(NULL, NULL, 0);
    context->CSSetShader(m_PSODesc.m_computeShader.Get(), NULL, 0);
}
void ComputePSO::SetComputeShader(
    const void *Binary, size_t Size,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    ThrowIfFailed(device->CreateComputeShader(
        Binary, Size, nullptr, m_PSODesc.m_computeShader.GetAddressOf()));
}
} // namespace soku