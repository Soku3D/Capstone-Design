#include "pch.h"
#include "GeometryGenerator.h"

#include "RenderApp.h"

namespace soku
{
	using DirectX::SimpleMath::Matrix;
	RenderApp::RenderApp(const int width, const int height) :
		BaseApp(width, height)
	{
	}
	bool RenderApp::Initialize()
	{
		if (!BaseApp::Initialize())
		{
			return false;
		}
		{
			cubeMap = std::make_shared <CubeMap>();
			cubeMap->Initialize(m_device,
				L"Assets/Textures/cubemaps/courtyard.dds",
				L"Assets/Textures/cubemaps/courtyard_diffuse.dds",
				L"Assets/Textures/cubemaps/courtyard_specural.dds");
		}
		return true;
	}
	void RenderApp::Update(float deltaTime)
	{
		if (keyDownState[int('A')] || keyDownState[VK_LEFT]) {
			m_camera->MoveRight(-deltaTime);
		}
		if (keyDownState[int('D')] || keyDownState[VK_RIGHT]) {
			m_camera->MoveRight(deltaTime);
		}
		if (keyDownState[int('W')] || keyDownState[VK_UP]) {
			m_camera->MoveForward(deltaTime);
		}
		if (keyDownState[int('S')] || keyDownState[VK_DOWN]) {
			m_camera->MoveForward(-deltaTime);
		}
		Matrix model = Matrix();
		Matrix view = m_camera->GetViewRow().Transpose();
		Matrix projection = m_camera->GetProjRow().Transpose();

		cubeMap->psConstantData = basicPSConstantData;
		cubeMap->vsConstantData.view = m_camera->GetCubeViewRow().Transpose();
		cubeMap->vsConstantData.projection = projection;
		cubeMap->UpdateConstantData(m_context);

		for (int i = 1; i <= 3; i++)
		{
			if (m_lightFlag != i) {
				basicPSConstantData.lights[i - 1].lightStrength = Vector3(0.f);
			}
			else {
				basicPSConstantData.lights[i - 1] = guiLight;
			}
		}
		basicPSConstantData.material.ambient = Vector3(guiAmbient);
		basicPSConstantData.material.diffuse = Vector3(guiDiffuse);
		basicPSConstantData.material.specular = Vector3(guiSpecular);

		basicPSConstantData.eyeWorld = m_camera->GetEyePos();

		for (auto& meshGroup : meshGroups)
		{
			meshGroup->drawNormal = drawMeshesNormal;
			meshGroup->vsConstantData.view = view;
			meshGroup->vsConstantData.projection = projection;
			basicPSConstantData.useTexture = meshGroup->psConstantData.useTexture;
			meshGroup->psConstantData = basicPSConstantData;
			meshGroup->UpdateConstantData(m_context);
		}
		
	}
	void RenderApp::UpdateGUI(float deltaTime)
	{
		ImGui::Checkbox("Draw NormalLines", &drawMeshesNormal);
		ImGui::Checkbox("Use Texture", &basicPSConstantData.useTexture);
		if (ImGui::RadioButton("Directional Light", m_lightFlag == 1)) {
			m_lightFlag = 1;
		}
		
	}
	void RenderApp::Render(float deltaTime)
	{
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

		m_context->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f, 0);
		// Multiple render targets
		ID3D11RenderTargetView* rtvs[]{
			m_renderTargetView.Get(),
			m_indexRtv.Get()
		};
		m_context->OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, m_depthStencilView.Get());
		m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		m_context->RSSetState(m_rasterizerState.Get());

		cubeMap->Render(m_context);
	}
}
