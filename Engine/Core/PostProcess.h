#pragma once
#include "ImageFilter.h"
#include "Model.h"
namespace soku {
class PostProcess {
  public:
    void Initialize(
        Microsoft::WRL::ComPtr<ID3D11Device> &device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
        const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &resources,
        const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &targets,
        const int &width, const int &height);

    void
    CreateBuffer(const int &width, const int &height,
                 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &outputSRV,
                 Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &outputRTV,
                 Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

  private:
    std::vector<ImageFilter> downFilters;
    std::vector<ImageFilter> upFilters;
    ImageFilter combineFilter;

  private:
    int m_indexCount;
    std::shared_ptr<Model> model;
  private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> SRVs;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RTVs;
};
} // namespace soku