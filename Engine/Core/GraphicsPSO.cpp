#include "pch.h"
#include "GraphicsPSO.h"

namespace soku {
    void GraphicsPSO::SetPipelineState(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
    {
        context->IASetPrimitiveTopology(m_PSODesc.m_topology);
        context->IASetInputLayout(m_PSODesc.m_inputLayout.Get());

        context->VSSetShader(m_PSODesc.m_vertexShader.Get(), NULL, 0);
        context->PSSetShader(m_PSODesc.m_pixelShader.Get(), NULL, 0);
        context->DSSetShader(m_PSODesc.m_domainShader.Get(), NULL, 0);
        context->HSSetShader(m_PSODesc.m_hullShader.Get(), NULL, 0);
        context->GSSetShader(m_PSODesc.m_geometryShader.Get(), NULL, 0);

        context->RSSetState(m_PSODesc.m_rasterizerState.Get());
        
        context->VSSetSamplers(0, 1, m_PSODesc.m_samplerState.GetAddressOf());
        context->PSSetSamplers(0, 1, m_PSODesc.m_samplerState.GetAddressOf());
    }
    void GraphicsPSO::SetSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState)
    {
        m_PSODesc.m_samplerState = samplerState;
    }
    void GraphicsPSO::SetRasterizerState(Microsoft::WRL::ComPtr<ID3D11RasterizerState>& rasterizerState)
    {
        m_PSODesc.m_rasterizerState = rasterizerState;
    }
    void GraphicsPSO::SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout)
    {
        m_PSODesc.m_inputLayout = inputLayout;
    }
    void GraphicsPSO::SetDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& depthStencilState)
    {
        m_PSODesc.m_depthStencilState = depthStencilState;
    }
    void GraphicsPSO::SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState>& blendState)
    {
        m_PSODesc.m_blendState = blendState;
    }
    void GraphicsPSO::SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
    {
        m_PSODesc.m_topology = topology;
    }
}