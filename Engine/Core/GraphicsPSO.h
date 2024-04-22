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
struct D3D11_COMPUTE_PIPELINE_STATE_DESC {
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};
class GraphicsPSO {
  public:
    void operator=(const GraphicsPSO &pso);
    void SetPipelineState(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void SetSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState> &samplerState);
    void SetRasterizerState(
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> &rasterizerState);
    void SetInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout);
    void SetDepthStencilState(
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> &depthStencilState);
    void SetBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> &blendState);
    void SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY &m_topology);

    void SetVertexShader(const void *Binary, size_t Size,
                         Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void SetPixelShader(const void *Binary, size_t Size,
                        Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void SetGeometryShader(const void *Binary, size_t Size,
                           Microsoft::WRL::ComPtr<ID3D11Device> &device);
   

  private:
    D3D11_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;
};
class ComputePSO {
  public:
    void operator=(const ComputePSO &pso);
    void SetPipelineState(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void SetSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState> &samplerState);
    void SetComputeShader(const void *Binary, size_t Size,
                          Microsoft::WRL::ComPtr<ID3D11Device> &device);
  private:
    D3D11_COMPUTE_PIPELINE_STATE_DESC m_PSODesc;
};
} // namespace soku