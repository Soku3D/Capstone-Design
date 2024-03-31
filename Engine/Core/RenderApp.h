#pragma once
#include "pch.h"
#include "BaseApp.h"
#include "Mesh.h"

#define MAX_LIGHTS 3
namespace soku {
	class RenderApp: public BaseApp {
	public:
		RenderApp(const int width, const int height);
		virtual bool Initialize() override;
	private:
		virtual void Render(float deltaTime) override;
		virtual void Update(float deltaTime) override;
		virtual void UpdateGUI(float deltaTime) override;
	private:
		BasicVSConstantData basicVSConstantData;
		BasicPSConstantData basicPSConstantData;

		std::vector<std::shared_ptr<MeshGroup>> meshGroups;
		std::shared_ptr <CubeMap> cubeMap;
	private:
		std::shared_ptr<class BillboardPoints> billboardPoints;
		DirectX::BoundingSphere m_boundingSphere;
		std::shared_ptr<MeshGroup> pickingMesh;
		std::shared_ptr<MeshGroup> lightMesh;
		bool m_selected;
		int m_lightFlag = 1;
		Light guiLight;
		float guiAmbient =  0.2f;
		float guiDiffuse = 1.f;
		float guiSpecular = 1.f;
		bool drawMeshesNormal = true;
	};
}