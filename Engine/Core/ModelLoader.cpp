#include "pch.h"
#include "ModelLoader.h"

namespace soku {
	void ModelLoader::Load(const std::string& filePath, const std::string& fileName)
	{
		this->filePath = filePath;
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(filePath + fileName,
			aiPostProcessSteps::aiProcess_Triangulate |	aiProcess_ConvertToLeftHanded);
		ProcessNode(pScene, pScene->mRootNode);
	}
	void ModelLoader::ProcessNode(const aiScene* pScene, aiNode* pNode)
	{
		for (int i = 0; i < pNode->mNumMeshes; i++) {
			m_meshData.push_back(ProcessMesh(pScene , pScene->mMeshes[pNode->mMeshes[i]]));
		}
		for (int i = 0; i < pNode->mNumChildren; i++)
		{
			ProcessNode(pScene, pNode->mChildren[i]);
		}
	}
	MeshData ModelLoader::ProcessMesh(const aiScene* pScene, aiMesh* pMesh)
	{
		MeshData data;
		for (int i = 0; i < pMesh->mNumVertices; i++)
		{
			Vertex v;
			v.position = Vector3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
			v.normal = Vector3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
			if (pMesh->mTextureCoords[0]) {
				v.uv.x = (float)pMesh->mTextureCoords[0][i].x;
				v.uv.y = (float)pMesh->mTextureCoords[0][i].y;
			}
			data.m_vertices.push_back(v);
		}
		for (int i = 0; i < pMesh->mNumFaces; i++)
		{
			for (int j = 0; j < pMesh->mFaces[i].mNumIndices; j++) {
				data.m_indices.push_back(pMesh->mFaces[i].mIndices[j]);
			}
		}
		if (pMesh->mMaterialIndex >=0) {
			aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString filepath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

				std::string fullPath =
					this->filePath +
					std::string(std::filesystem::path(filepath.C_Str())
						.filename()
						.string());
				data.texturePath = fullPath;
			}
		}
		return data;
	}
}