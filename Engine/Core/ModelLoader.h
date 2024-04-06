#pragma once
#include "MeshData.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

namespace soku {
	class ModelLoader {
	public:
		ModelLoader(){}
		void Load(const std::string& filePath, const std::string& fileName);
		void ProcessNode(const aiScene* pScene, aiNode* pNode);
		MeshData ProcessMesh(const aiScene* pScene, aiMesh* pMesh);

	public:
		std::vector<MeshData> m_meshData;
		std::string filePath;
	};
}