#pragma once
#include "ImageFilter.h"
#include "Mesh.h"

namespace soku {
class PostProcess {
  public:
    void Initialize(
        Microsoft::WRL::ComPtr<ID3D11Device> &device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
        const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &resources,
        const std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> &targets,
        const int &width, const int &height, const int &bloomLevel = 5);

    void
    CreateBuffer(const int &width, const int &height,
                 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &outputSRV,
                 Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &outputRTV,
                 Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
    void Update(const SamplingPSConstants &constant,
                Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
  private:
    std::vector<ImageFilter> downFilters;
    std::vector<ImageFilter> upFilters;
    ImageFilter combineFilter;

  private:
    int m_indexCount;
    std::shared_ptr<Mesh> m_meshes;
  private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> SRVs;
    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> RTVs;
};
} // namespace soku