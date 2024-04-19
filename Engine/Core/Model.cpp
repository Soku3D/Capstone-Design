#include "pch.h"
#include "Model.h"
#include "Utils.h"
namespace soku {
Model::Model() {}
Model::Model(Microsoft::WRL::ComPtr<ID3D11Device> &device,
             Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
             const std::vector<MeshData> &data, const std::string &filename,
             const std::string &fileExtension, const std::string &basePath) {
    Initialize(device, context, data, filename, fileExtension, basePath);
}
void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                       Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &data,
                       const std::string &filename,
                       const std::string &fileExtension,
                       const std::string &basePath) {
    Initialize(device, context, data);
}

void Model::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device,
                       Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &data) {

    m_meshConstantsCPU.world = Matrix();
    Utils::CreateConstantBuffer(m_meshConstantsCPU, m_meshConstantsGPU, device);
    
    m_materialConstantsCPU.useAlbedo = false;
    m_materialConstantsCPU.useNormal = false;
    m_materialConstantsCPU.useAO = false;
    m_materialConstantsCPU.useRoughness = false;
    Utils::CreateConstantBuffer(m_materialConstantsCPU, m_materialConstantsGPU,
                                device);
    for (const auto &mesh : data) {
        auto newMesh = std::make_shared<Mesh>();
        Utils::CreateVertexBuffer(mesh.m_vertices, newMesh->m_vertexBuffer,
                                  device);
        Utils::CreateIndexBuffer(mesh.m_indices, newMesh->m_indexBuffer,
                                 device);
        newMesh->m_indexCount = (UINT)mesh.m_indices.size();

        if (!mesh.albedoPath.empty()) {
            Utils::CreateTexture(mesh.albedoPath, newMesh->m_albedoTexture,
                                 newMesh->m_albedoSRV, device, context, true);
        }
        if (!mesh.aoPath.empty()) {
            Utils::CreateTexture(mesh.aoPath, newMesh->m_aoTexture,
                                 newMesh->m_aoSRV, device, context, true);
        }
        if (!mesh.heightPath.empty()) {
            Utils::CreateTexture(mesh.heightPath, newMesh->m_heightTexture,
                                 newMesh->m_heightSRV, device, context, true);
        }
        if (!mesh.normalPath.empty()) {
            Utils::CreateTexture(mesh.normalPath, newMesh->m_normalTexture,
                                 newMesh->m_normalSRV, device, context, true);
        }
        m_meshes.push_back(newMesh);
    }
}
void Model::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
    context->VSSetConstantBuffers(0, 1, m_meshConstantsGPU.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_materialConstantsGPU.GetAddressOf());
    for (const auto &mesh : m_meshes) {
        UINT stride = sizeof(Vertex);
        UINT offsets = 0;

        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(),
                                    &stride, &offsets);
        context->IASetIndexBuffer(mesh->m_indexBuffer.Get(),
                                  DXGI_FORMAT_R32_UINT, 0);
        
        context->VSSetShaderResources(0, 1, mesh->m_heightSRV.GetAddressOf());
        
        std::vector<ID3D11ShaderResourceView *> srvs = {
            mesh->m_albedoSRV.Get(), mesh->m_aoSRV.Get(),
            mesh->m_normalSRV.Get()};
        context->PSSetShaderResources(0, (UINT)srvs.size(), srvs.data());
        
        context->DrawIndexed(mesh->m_indexCount, 0, 0);
        context->Draw(mesh->m_indexCount, 0);
    }
}
} // namespace soku
