#include "pch.h"
#include "Model.h"
#include "Utils.h"
namespace soku {
Model::Model() {}
void Model::Initialize(const std::vector<MeshData> &data,
                       Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    m_meshConstantsCPU.world = Matrix();
    Utils::CreateConstantBuffer(m_meshConstantsCPU, m_meshConstantsGPU, device);
    for (const auto &mesh : data) {
        auto newMesh = std::make_shared<Mesh>();
        Utils::CreateVertexBuffer(mesh.m_vertices, newMesh->m_vertexBuffer,
                                  device);
        Utils::CreateIndexBuffer(mesh.m_indices, newMesh->m_indexBuffer,
                                 device);
        newMesh->m_indexCount = mesh.m_indices.size();

        m_meshes.push_back(newMesh);
    }
}
void Model::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    context->VSSetConstantBuffers(0, 1, m_meshConstantsGPU.GetAddressOf());
    for (const auto &mesh : m_meshes) {
        UINT stride = sizeof(Vertex);
        UINT offsets = 0;

        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(),
                                    &stride, &offsets);
        context->IASetIndexBuffer(mesh->m_indexBuffer.Get(),
                                  DXGI_FORMAT_R32_UINT, 0);
        //context->PSSetShaderResources(0, 1, mesh->m_albedoSRV.GetAddressOf());

        context->DrawIndexed(mesh->m_indexCount, 0, 0);
    }
}
} // namespace soku
