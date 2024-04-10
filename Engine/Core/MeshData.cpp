#include "pch.h"
#include "MeshData.h"
namespace soku {
MeshData::MeshData(const std::vector<Vertex> &vertices,
                   const std::vector<uint32_t> &indices) {
    m_vertices = vertices;
    m_indices = indices;
}
void MeshData::SetTexturePath(const std::wstring &filename,
                              const std::wstring &fileExtension,
                              const std::wstring &basePath) {

    albedoPath = basePath + L"/" + filename + L"/" + filename + L"_albedo." +
                 fileExtension;
    aoPath     = basePath + L"/" + filename + L"/" + filename + L"_ao." + 
                 fileExtension;
    heightPath = basePath + L"/" + filename + L"/" + filename + L"_height." +
                 fileExtension;
    normalPath = basePath + L"/" + filename + L"/" + filename + L"_normal." +
                 fileExtension;
}
} // namespace soku
