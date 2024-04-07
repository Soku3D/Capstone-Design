#pragma once
#include "pch.h"
#include "BaseApp.h"
#include "Mesh.h"
#include "Model.h"
#define MAX_LIGHTS 3
namespace soku {
	class RenderApp : public BaseApp {
	public:
		RenderApp(const int width, const int height);
		virtual bool Initialize() override;
	private:
		virtual void Render(float deltaTime) override;
		virtual void Update(float deltaTime) override;
		virtual void UpdateGUI(float deltaTime) override;
	private:
		std::vector<std::shared_ptr<Model>> models;
	private:
		std::shared_ptr<class BillboardPoints> billboardPoints;
		DirectX::BoundingSphere m_boundingSphere;
	};
}