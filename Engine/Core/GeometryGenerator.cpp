#include "pch.h"
#include "GeometryGenerator.h"
#include "ModelLoader.h"


namespace soku {
MeshData GeometryGenerator::MakeSquare() {
    MeshData meshData;
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    float l = 1.f;
    Vector3 p0(-l, -l, 0.f);
    Vector3 p1(-l, l, 0.f);
    Vector3 p2(l, l, 0.f);
    Vector3 p3(l, -l, 0.f);
    Vector3 n(0, 0, -l);

    std::vector<Vector3> points{
        p0,
        p1,
        p2,
        p3,
    };
    for (int i = 0; i < 1; i++) {
        Vertex v0{points[i], Vector2(0.f, 1.f), n};
        Vertex v1{points[i + 1], Vector2(0.f, 0.f), n};
        Vertex v2{points[i + 2], Vector2(1.f, 0.f), n};
        Vertex v3{points[i + 3], Vector2(1.f, 1.f), n};
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    meshData.m_vertices = vertices;
    meshData.m_indices = indices;

    return meshData;
}
MeshData GeometryGenerator::MakeCubeMapBox(float l) {
    MeshData data;
    return data;
}
MeshData GeometryGenerator::MakeBox(float l, const std::string &texturePath) {
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    Vector3 p0(-l, l, -l);
    Vector3 p1(-l, l, l);
    Vector3 p2(l, l, l);
    Vector3 p3(l, l, -l);
    Vector3 p4(-l, -l, -l);
    Vector3 p5(-l, -l, l);
    Vector3 p6(l, -l, l);
    Vector3 p7(l, -l, -l);

    Vector3 n0(0, l, 0);
    Vector3 n1(0, -l, 0);
    Vector3 n2(l, 0, 0);
    Vector3 n3(-l, 0, 0);
    Vector3 n4(0, 0, -l);
    Vector3 n5(0, 0, l);

    std::vector<Vector3> norms{n0, n1, n2, n3, n4, n5};
    std::vector<Vector3> points{p0, p1, p2, p3, p5, p4, p7, p6, p7, p3, p2, p6,
                                p5, p1, p0, p4, p4, p0, p3, p7, p6, p2, p1, p5};
    for (int i = 0; i < 6; i++) {
        Vertex v0{points[i * 4], Vector2(0.f, 1.f), norms[i]};
        Vertex v1{points[i * 4 + 1], Vector2(0.f, 0.f), norms[i]};
        Vertex v2{points[i * 4 + 2], Vector2(1.f, 0.f), norms[i]};
        Vertex v3{points[i * 4 + 3], Vector2(1.f, 1.f), norms[i]};
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }
    for (int j = 0; j < 6; j++) {
        int i = 4 * j;
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }
    return MeshData(vertices, indices);
}
MeshData GeometryGenerator::MakeGrid(int x, int y) {
    MeshData meshData;
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    float l = 1.f;
    Vector3 point(-l, l, 0.f);
    Vector2 uvPoint(0, 0);
    float dx = (2.f * l) / x;
    float dy = -(2.f * l) / y;
    float uvDx = l / x;
    float uvDy = l / y;
    Vector3 d(dx, dy, 0.f);
    for (int height = 0; height <= y; height++) {
        for (int width = 0; width <= x; width++) {
            Vector3 p = point + Vector3(width * dx, height * dy, 0.f);
            Vector2 uv = uvPoint + Vector2(width * uvDx, height * uvDy);
            Vector3 normal(0, 0, -1);
            Vertex v0{p, uv, normal};
            vertices.push_back(v0);
        }
    }
    for (int height = 0; height < y; height++) {
        for (int width = 0; width < x; width++) {
            int idx1 = width + (x + 1) * height;

            int idx0 = width + (x + 1) * (height + 1);
            int idx2 = idx1 + 1;
            int idx3 = idx0 + 1;
            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx3);
        }
    }
    meshData.m_vertices = vertices;
    meshData.m_indices = indices;
    return meshData;
}
MeshData GeometryGenerator::MakeCylinder(int x, int y, float height,
                                         int radius) {
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    Vector3 point(-radius, height / 2.f, 0.f);
    Vector2 uvPoint(0, 0);
    float delTheta = -DirectX::XM_2PI / x;
    float dy = -(height) / y;
    float uvDx = 1.f / x;
    float uvDy = 1.f / y;

    for (int height = 0; height <= y; height++) {
        for (int width = 0; width <= x; width++) {
            Vector3 p = point + Vector3(0, dy * height, 0.f);
            p = Vector3::Transform(
                p,
                DirectX::SimpleMath::Matrix::CreateRotationY(delTheta * width));
            Vector2 uv = uvPoint + Vector2(width * uvDx, height * uvDy);
            Vector3 normal(p.x, 0, p.z);
            normal.Normalize();
            Vertex v0{p, uv, normal};
            vertices.push_back(v0);
        }
    }
    for (int height = 0; height < y; height++) {
        for (int width = 0; width < x; width++) {
            int idx1 = width + (x + 1) * height;

            int idx0 = width + (x + 1) * (height + 1);
            int idx2 = idx1 + 1;
            int idx3 = idx0 + 1;
            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx3);
        }
    }
    return MeshData(vertices, indices);
}
MeshData GeometryGenerator::MakeSphere(int x, int y, float radius,
                                       const std::string &texturePath) {
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    Vector3 point(0, radius, 0.f);
    Vector2 uvPoint(0, 0);
    float delThetaX = -DirectX::XM_2PI / x;
    float delThetaY = DirectX::XM_PI / y;
    float uvDx = 1.f / x;
    float uvDy = 1.f / y;

    for (int height = 0; height <= y; height++) {
        for (int width = 0; width <= x; width++) {
            Vector3 p = Vector3::Transform(
                point, DirectX::SimpleMath::Matrix::CreateRotationZ(delThetaY *
                                                                    height));
            p = Vector3::Transform(p,
                                   DirectX::SimpleMath::Matrix::CreateRotationY(
                                       delThetaX * width));
            Vector2 uv = uvPoint + Vector2(width * uvDx, height * uvDy);
            Vector3 normal(p.x, p.y, p.z);
            normal.Normalize();
            Vertex v0{p, uv, normal};
            vertices.push_back(v0);
        }
    }
    for (int height = 0; height < y; height++) {
        for (int width = 0; width < x; width++) {
            int idx1 = width + (x + 1) * height;

            int idx0 = width + (x + 1) * (height + 1);
            int idx2 = idx1 + 1;
            int idx3 = idx0 + 1;
            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx3);
        }
    }
    return MeshData(vertices, indices, texturePath);
}
MeshData GeometryGenerator::MakeTetrahedron() {
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    Vertex v0, v1, v2, v3;
    v0.position = Vector3(0, sqrtf(6) / 2.f, 0);
    v0.normal = v0.position;
    v0.normal.Normalize();
    v1.position = Vector3(2.f * sqrtf(3) / 3.f, -sqrtf(6) / 6.f, 0);
    v1.normal = v1.position;
    v1.normal.Normalize();
    v2.position = Vector3(-sqrtf(3) / 3.f, -sqrtf(6) / 6.f, -1.f);
    v2.normal = v2.position;
    v2.normal.Normalize();
    v3.position = Vector3(-sqrtf(3) / 3.f, -sqrtf(6) / 6.f, 1.f);
    v3.normal = v3.position;
    v3.normal.Normalize();
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(1);

    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(2);

    return MeshData(vertices, indices);
}
MeshData GeometryGenerator::MakeIcosahedron() {
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    return MeshData(vertices, indices);
}
// MeshData GeometryGenerator::Subdivision(const meshData& meshData, float
// radius)
//{
//	std::vector<uint32_t> indices;
//	std::vector<Vertex> vertices;

//	int triangleCount = meshData.m_indices.size() / 3;
//	for (int i = 0; i < triangleCount; i++)
//	{
//		Vertex v0 = meshData.m_vertices[meshData.m_indices[i * 3]];
//		Vertex v1 = meshData.m_vertices[meshData.m_indices[i * 3 + 1]];
//		Vertex v2 = meshData.m_vertices[meshData.m_indices[i * 3 + 2]];

//		Vertex v3{ v0.position * 0.5f + v1.position * 0.5f,v0.uv * 0.5f + v1.uv
//* 0.5f, Vector3(0.f) }; 		Vertex v4{ v1.position * 0.5f + v2.position *
//0.5f,v1.uv * 0.5f + v2.uv * 0.5f, Vector3(0.f) }; 		Vertex v5{ v0.position *
//0.5f + v2.position * 0.5f,v0.uv * 0.5f + v2.uv * 0.5f, Vector3(0.f) };

//		vertices.push_back(v0);
//		vertices.push_back(v1);
//		vertices.push_back(v2);
//		vertices.push_back(v3);
//		vertices.push_back(v4);
//		vertices.push_back(v5);

//		int idx = 6 * i;
//		indices.push_back(idx);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 4);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 4);
//		indices.push_back(idx + 2);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 1);
//		indices.push_back(idx + 4);
//	}
//	for (auto& v : vertices) {
//		float l = v.position.Length();
//		v.position *= (radius / l);
//		v.normal = v.position;
//		v.normal.Normalize();
//	}
//	return MeshData(vertices, indices);
//}
// meshData GeometryGenerator::SubdivisionToSphere(const meshData& meshData,
// float radius)
//{
//	std::vector<uint32_t> indices;
//	std::vector<Vertex> vertices;

//	auto TranslateToSphere = [&](Vertex& v) {
//		v.normal = v.position;
//		v.normal.Normalize();
//		v.position = v.normal * radius;

//		/*float yTheta = acosf(v.position.y / radius);
//		float xTheta = atan2(v.position.x,v.position.z);
//		xTheta += DirectX::XM_PI;
//		yTheta /= DirectX::XM_PI;
//		xTheta /= DirectX::XM_2PI;

//		v.uv = Vector2(xTheta, yTheta);*/

//		};
//	int triangleCount = meshData.m_indices.size() / 3;
//	for (int i = 0; i < triangleCount; i++)
//	{
//		Vertex v0 = meshData.m_vertices[meshData.m_indices[i * 3]];
//		TranslateToSphere(v0);
//		Vertex v1 = meshData.m_vertices[meshData.m_indices[i * 3 + 1]];
//		TranslateToSphere(v1);
//		Vertex v2 = meshData.m_vertices[meshData.m_indices[i * 3 + 2]];
//		TranslateToSphere(v2);

//		Vertex v3{ v0.position * 0.5f + v1.position * 0.5f,v0.uv * 0.5f + v1.uv
//* 0.5f, Vector3(0.f) }; 		Vertex v4{ v1.position * 0.5f + v2.position *
//0.5f,v1.uv * 0.5f + v2.uv * 0.5f, Vector3(0.f) }; 		Vertex v5{ v0.position *
//0.5f + v2.position * 0.5f,v0.uv * 0.5f + v2.uv * 0.5f, Vector3(0.f) };
//		TranslateToSphere(v3);
//		TranslateToSphere(v4);
//		TranslateToSphere(v5);

//		vertices.push_back(v0);
//		vertices.push_back(v1);
//		vertices.push_back(v2);
//		vertices.push_back(v3);
//		vertices.push_back(v4);
//		vertices.push_back(v5);

//		int idx = 6 * i;
//		indices.push_back(idx);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 4);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 5);
//		indices.push_back(idx + 4);
//		indices.push_back(idx + 2);
//		indices.push_back(idx + 3);
//		indices.push_back(idx + 1);
//		indices.push_back(idx + 4);
//	}
//	return MeshData(vertices, indices);
//}
std::vector<MeshData> GeometryGenerator::LoadMeshData(std::string filePath,
                                                      std::string filename) {
    using namespace DirectX;

    ModelLoader modelLoader;
    modelLoader.Load(filePath, filename);
    std::vector<MeshData> &meshes = modelLoader.m_meshData;

    // Normalize vertices
    Vector3 vmin(1000, 1000, 1000);
    Vector3 vmax(-1000, -1000, -1000);
    for (auto &mesh : meshes) {
        for (auto &v : mesh.m_vertices) {
            vmin.x = XMMin(vmin.x, v.position.x);
            vmin.y = XMMin(vmin.y, v.position.y);
            vmin.z = XMMin(vmin.z, v.position.z);
            vmax.x = XMMax(vmax.x, v.position.x);
            vmax.y = XMMax(vmax.y, v.position.y);
            vmax.z = XMMax(vmax.z, v.position.z);
        }
    }

    float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
    float dl = XMMax(XMMax(dx, dy), dz);
    float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
          cz = (vmax.z + vmin.z) * 0.5f;

    for (auto &mesh : meshes) {
        for (auto &v : mesh.m_vertices) {
            v.position.x = (v.position.x - cx) / dl;
            v.position.y = (v.position.y - cy) / dl;
            v.position.z = (v.position.z - cz) / dl;
        }
    }

    return meshes;
}
} // namespace soku