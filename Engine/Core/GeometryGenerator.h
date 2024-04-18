#pragma once
#include "MeshData.h"
namespace soku {

class GeometryGenerator {
  public:
    static MeshData MakeDot();
    static MeshData MakeSquare(const float &l = 1.f);
    static MeshData MakeCubeMapBox(float l = 1.f);
    static MeshData MakeBox(float l = 1.f, const std::string &texturePath = "");
    static MeshData MakeGrid(int x = 1, int y = 1);
    static MeshData MakeCylinder(int x = 5, int y = 2, float height = 2.f,
                                 float radius = 1.f);
    static MeshData MakeSphere(int x = 5, int y = 5, float radius = 1.f,
                               const std::string &texturePath = "");
    static MeshData MakeTetrahedron();
    static MeshData MakeIcosahedron();
    // static MeshData Subdivision(const MeshData& meshData, float radius
    // = 1.f); static MeshData SubdivisionToSphere(const MeshData& meshData,
    // float radius = 1.f);
    static std::vector<MeshData> LoadMeshData(std::string filePath,
                                              std::string filename);
};
} // namespace soku