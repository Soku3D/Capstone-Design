#pragma once
namespace soku {
struct D3D11_GRAPHICS_PIPELINE_STATE_DESC {
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;
    Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

    D3D11_PRIMITIVE_TOPOLOGY m_topology;
};
class GraphicsPSO {
  public:
    void operator=(const GraphicsPSO &pso);
    void SetPipelineState(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void
    SetSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState> &samplerState);
    void SetRasterizerState(
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> &rasterizerState);
    void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout);
    void SetDepthStencilState(
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> &depthStencilState);
    void SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> &blendState);
    void SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY &m_topology);

    void SetVertexShader(const void *Binary, size_t Size,
                         Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        device->CreateVertexShader(Binary, Size, nullptr,
                                   m_PSODesc.m_vertexShader.GetAddressOf());
    }
    void SetPixelShader(const void *Binary, size_t Size,
                        Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        device->CreatePixelShader(Binary, Size, nullptr,
                                  m_PSODesc.m_pixelShader.GetAddressOf());
    }

  private:
    D3D11_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;
};
} // namespace soku