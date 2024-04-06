#pragma once
#include <vector>
#include <string>
#include "Vertex.h"
namespace soku {
	struct MeshData {
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::string texturePath;
	};
}
