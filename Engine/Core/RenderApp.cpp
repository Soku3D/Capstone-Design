#include "pch.h"
#include "RenderApp.h"
#include "GeometryGenerator.h"

namespace soku {
using DirectX::SimpleMath::Matrix;
RenderApp::RenderApp(const int width, const int height)
    : BaseApp(width, height) {}
bool RenderApp::Initialize() {
    if (!BaseApp::Initialize()) {
        return false;
    }
    BaseApp::InitCubemaps(L"Assets/Textures/Skybox/Sample/", L"Sample");

    auto sphere = GeometryGenerator::MakeSphere(100, 100);
    sphere.SetTexturePath(L"greyRock");
    auto box = GeometryGenerator::MakeBox(40.f);

    skybox = std::make_shared<Model>(m_device, m_context, std::vector{box});
    auto sphereModel =
        std::make_shared<Model>(m_device, m_context, std::vector{sphere});
    models.push_back(sphereModel);
    /* auto boxData = GeometryGenerator::MakeBox();
    auto grid = std::make_shared<Model>();
    grid->Initialize({boxData}, m_device);
    models.push_back(grid);*/

    return true;
}
void RenderApp::Update(float deltaTime) {
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
    Matrix viewRow = m_camera->GetViewRow();
    Matrix projRow = m_camera->GetProjRow();
    Vector3 eyeWorld = m_camera->GetEyePos();

    BaseApp::UpdateGlobalConsts(eyeWorld, viewRow, projRow, textureLOD);
    for (const auto &model : models) {
        model->m_meshConstantsCPU.world =
            Matrix::CreateTranslation(Vector3(0.f, 0.f, 1.5f));
        model->m_meshConstantsCPU.worldIT =
            model->m_meshConstantsCPU.world.Invert().Transpose();
        model->m_meshConstantsCPU.world =
            model->m_meshConstantsCPU.world.Transpose();
        model->m_meshConstantsCPU.worldIT =
            model->m_meshConstantsCPU.worldIT.Transpose();
        model->m_meshConstantsCPU.heightScale = heightScale;
        Utils::UpdateConstantBuffer(model->m_meshConstantsCPU,
                                    model->m_meshConstantsGPU, m_context);
    }
    m_postProcess.Update(SPSconstant, m_context);
}
void RenderApp::UpdateGUI(float deltaTime) {
    ImGui::SliderFloat("TextureLOD", &textureLOD, 0.f, 10.f);
    ImGui::SliderFloat("HeightScale", &heightScale, 0.f, 1.f);
    ImGui::SliderFloat("bloomStrength", &SPSconstant.bloomStrength, 0.f, 1.f);
}

void RenderApp::Render(float deltaTime) {
    FLOAT color[4]{80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f};
    m_context->OMSetRenderTargets(1, m_floatRTV.GetAddressOf(), m_DSV.Get());
    m_context->ClearRenderTargetView(m_floatRTV.Get(), color);
    m_context->ClearDepthStencilView(
        m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    BaseApp::SetGlobalConsts();
    m_context->PSSetShaderResources(10, 1, m_envSRV.GetAddressOf());

    Graphics::defaultSolidPSO.SetPipelineState(m_context);

    for (const auto &model : models) {
        model->Render(m_context);
    }

    Graphics::skyboxPSO.SetPipelineState(m_context);
    skybox->Render(m_context);

    m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);
    

    m_postProcess.Render(m_context);
}
} // namespace soku
