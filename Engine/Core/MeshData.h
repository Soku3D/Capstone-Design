#pragma once
#include <vector>
#include <string>
#include "Vertex.h"
namespace soku {
	class MeshData {
	public:
		MeshData():
			albedoTextureFilename("")
		{
		}
		MeshData(const std::vector<Vertex>& vertices,
			const std::vector<uint32_t>& indices,
			const std::string& albedo = "");
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::string albedoTextureFilename;
	};
}
