#pragma once
#include "Constants.h"
#include "Mesh.h"
#include "MeshData.h"
namespace soku {
class Model {
  public:
    Model();
    Model(Microsoft::WRL::ComPtr<ID3D11Device> &device,
          Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &data, const std::string &filename = "",
          const std::string &fileExtension = "",
          const std::string &basePath = "");

    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                    const std::vector<MeshData> &data,
                    const std::string &filename,
                    const std::string &fileExtension,
                    const std::string &basePath);

    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                    const std::vector<MeshData> &data);

    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

  public:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    MeshConstants m_meshConstantsCPU;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshConstantsGPU;
};
} // namespace soku