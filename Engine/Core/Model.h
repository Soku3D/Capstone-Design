#pragma once
#include "Constants.h"
#include "Mesh.h"
#include "MeshData.h"
namespace soku {
class Model {
  public:
    Model();
    void Initialize(const std::vector<MeshData> &data,
                    Microsoft::WRL::ComPtr<ID3D11Device> &device);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);

  public:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    MeshConstants m_meshConstantsCPU;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_meshConstantsGPU;
};
} // namespace soku