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

		/*{
			auto zeldaData = GeometryGenerator::LoadMeshData("Assets/models/zelda/", "zeldaPosed001.fbx");
			auto zeldaMesh = std::make_shared<MeshGroup>();
			zeldaMesh->Initialize(m_device, zeldaData, L"Basic", L"Basic");
			zeldaMesh->psConstantData.useTexture = true;
			zeldaMesh->UpdateConstantData(m_context);
			meshGroups.push_back(zeldaMesh);
		}*/
		//Create MeshData & Set TexturePath
		//{
		//	meshData sphereMeshData = GeometryGenerator::MakeSphere(100, 100, 1.f, "Assets/Textures/map.jpg");
		//	std::vector<meshData> meshes_{ sphereMeshData };
		//	auto sphereMeshGroup = std::make_shared<MeshGroup>();
		//	sphereMeshGroup->Initialize(m_device, meshes_, L"Basic", L"Basic");
		//	sphereMeshGroup->cubemapOrigin = cubeMap->cubemapOrigin;
		//	sphereMeshGroup->cubemapDiffuse = cubeMap->cubemapDiffuse;
		//	sphereMeshGroup->cubemapSpecular = cubeMap->cubemapSpecular;
		//	//sphereMeshGroup->UpdateModelWorld(Matrix::CreateTranslation(center));
		//	//sphereMeshGroup->UpdateConstantData(m_context);
		//	meshGroups.push_back(sphereMeshGroup);
		//}
	
		/*pickingMesh = std::make_shared<MeshGroup>();
		pickingMesh->Initialize(m_device, { GeometryGenerator::MakeSphere(15, 15, 0.1f) }, L"Basic", L"Basic");
		
		lightMesh = std::make_shared<MeshGroup>();
		lightMesh->Initialize(m_device, { GeometryGenerator::MakeSphere(15, 15, 0.1f) }, L"Basic", L"Basic");*/

		
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
		/*lightMesh->vsConstantData.model = Matrix::CreateTranslation(guiLight.lightPos).Transpose();
		lightMesh->vsConstantData.view = view;
		lightMesh->vsConstantData.projection = projection;
		lightMesh->UpdateConstantData(m_context);*/
	}
	void RenderApp::UpdateGUI(float deltaTime)
	{
		ImGui::Checkbox("Draw NormalLines", &drawMeshesNormal);
		ImGui::Checkbox("Use Texture", &basicPSConstantData.useTexture);
		if (ImGui::RadioButton("Directional Light", m_lightFlag == 1)) {
			m_lightFlag = 1;
		}
		ImGui::SameLine(); 
		if (ImGui::RadioButton("Point Light", m_lightFlag == 2)) {
			m_lightFlag = 2;
		}
		ImGui::SameLine(); 
		if (ImGui::RadioButton("Spot Light", m_lightFlag == 3)) {
			m_lightFlag = 3;
		}
		ImGui::SliderFloat3("Light Position", &guiLight.lightPos.x, -10.f, 10.f);
		ImGui::SliderFloat3("Light Direction", &guiLight.lightDir.x, -1.f, 1.f);
		ImGui::SliderFloat("FallOfStart", &guiLight.fallOffStart, 0.f, 100.f);
		ImGui::SliderFloat("FallOfEnd", &guiLight.fallOffEnd, 0.f, 100.f);

		ImGui::SliderFloat("Ambient", &guiAmbient, 0.f, 1.f);
		ImGui::SliderFloat("Diffuse", &guiDiffuse, 0.f, 1.f);
		ImGui::SliderFloat("Specural", &guiSpecular, 0.f, 1.f);
		ImGui::SliderFloat3("Material Fresnel", &basicPSConstantData.material.fresnelR0.x, 0.f, 1.f);
		ImGui::SliderFloat("Material Shininess", &basicPSConstantData.material.shininess, 0.f, 100.f);
		ImGui::SliderFloat("Spot Power", &guiLight.spotPower, 0.f, 100.f);

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

		/*for (auto meshGroup : meshGroups) {
			meshGroup->Render(m_context);
		}*/
		cubeMap->Render(m_context);
		/*if (m_selected && mouse->GetState().leftButton)
		{
			pickingMesh->Render(m_context);
		}
		lightMesh->Render(m_context);
		*/
	}
}
