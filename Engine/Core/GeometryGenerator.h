#pragma once
#include "Mesh.h"
namespace soku {
	
	class GeometryGenerator {
	public:
		static meshData MakePlane();
		static MeshData<SimpleVertex, uint32_t> MakeCubeMapBox(float l = 1.f);
		static meshData MakeBox(float l = 1.f, const std::string& texturePath = "");
		static meshData MakeGrid(int x = 1, int y = 1);
		static meshData MakeCylinder(int x = 5, int y = 2, float height = 2.f, int radius = 1.f);
		static meshData MakeSphere(int x = 5, int y = 5, float radius = 1.f, const std::string& texturePath = "");
		static meshData MakeTetrahedron();
		static meshData MakeIcosahedron();
		static meshData Subdivision(const meshData& meshData, float radius = 1.f);
		static meshData SubdivisionToSphere(const meshData& meshData, float radius = 1.f);
		static std::vector<meshData> LoadMeshData(std::string filePath, std::string filename);
	};
}