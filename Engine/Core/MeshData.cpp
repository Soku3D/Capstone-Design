#include "pch.h"
#include "MeshData.h"
namespace soku {
	MeshData::MeshData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& albedo)
	{
		m_vertices = vertices;
		m_indices = indices;
		albedoTextureFilename = albedo;
	}
}
