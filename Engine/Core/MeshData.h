#pragma once
#include "Vertex.h"
#include <string>
#include <vector>
namespace soku {
class MeshData {
  public:
    MeshData() {}
    MeshData(const std::vector<Vertex> &vertices,
             const std::vector<uint32_t> &indices);
    void SetTexturePath(const std::wstring &filename,
                        const std::wstring &fileExtension = L"jpg",
                        const std::wstring &basePath = L"Assets/Textures");
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    std::wstring albedoPath;
    std::wstring aoPath;
    std::wstring heightPath;
    std::wstring normalPath;
};
} // namespace soku
