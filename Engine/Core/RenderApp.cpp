#include "pch.h"
#include "RenderApp.h"
#include "GeometryGenerator.h"
#include "GraphicsCommon.h" 
namespace soku {
using DirectX::SimpleMath::Matrix;
RenderApp::RenderApp(const int width, const int height)
    : BaseApp(width, height) {}
bool RenderApp::Initialize() {
    if (!BaseApp::Initialize()) {
        return false;
    }
    BaseApp::InitCubemaps(L"Assets/Textures/Skybox/Sample/", L"Sample");

    // Create SkyBox
    auto box = GeometryGenerator::MakeBox(40.f);
    skybox = std::make_shared<Model>(m_device, m_context, std::vector{box});

    // Create SphereModel
    auto sphere = GeometryGenerator::MakeSphere(100, 100);
    // sphere.SetTexturePath(L"greyRock");
    // sphere.SetTexturePath(L"globe");
    auto sphereModel =
        std::make_shared<Model>(m_device, m_context, std::vector{sphere});
    sphereModel->m_materialConstantsCPU.useAlbedo = true;
    Utils::UpdateConstantBuffer(sphereModel->m_materialConstantsCPU,
                                sphereModel->m_materialConstantsGPU, m_context);
    // models.push_back(sphereModel);

    // Create Mirror
    auto square = GeometryGenerator::MakeSquare(3.f);
    mirror = std::make_shared<Model>(m_device, m_context, std::vector{square});

    auto square2 = GeometryGenerator::MakeSquare(1.f);
    screen = std::make_shared<Model>(m_device, m_context, std::vector{square2});

    MeshData dot = GeometryGenerator::MakeDot();
    dotModel = std::make_shared<Model>(m_device, m_context, std::vector{dot});

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

    // Update mirror Meshconstants
    Vector3 mirrorTranslation(0.f, 1.f, 1.3f);
    mirror->m_meshConstantsCPU.world =
        Matrix::CreateTranslation(mirrorTranslation);
    mirror->m_meshConstantsCPU.worldIT =
        mirror->m_meshConstantsCPU.world.Invert().Transpose();
    mirror->m_meshConstantsCPU.world =
        mirror->m_meshConstantsCPU.world.Transpose();
    mirror->m_meshConstantsCPU.worldIT =
        mirror->m_meshConstantsCPU.worldIT.Transpose();
    mirror->m_meshConstantsCPU.heightScale = heightScale;
    Utils::UpdateConstantBuffer(mirror->m_meshConstantsCPU,
                                mirror->m_meshConstantsGPU, m_context);
    mirrorMat = Utils::CreateReflectedMatrix(Vector3(0.f, 0.f, -1.f),
                                             mirrorTranslation);
    BaseApp::UpdateGlobalConsts(eyeWorld, viewRow, projRow, textureLOD,
                                mirrorMat, 1, deltaTime);

    // Update Model Constants
    for (const auto &model : models) {
        model->m_meshConstantsCPU.world = Matrix();
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

    // Update Screen Model
    /*screen->m_meshConstantsCPU.world = Matrix();
    screen->m_meshConstantsCPU.world =
        mirror->m_meshConstantsCPU.world.Transpose();

    Utils::UpdateConstantBuffer(screen->m_meshConstantsCPU,
                                screen->m_meshConstantsGPU, m_context);*/

    // Update Combile Filter Constants
    m_postProcess.Update(SPSconstant, m_context);
}
void RenderApp::UpdateGUI(float deltaTime) {
    if (ImGui::RadioButton("Env", useEnv == 1)) {
        useEnv = 1;
    }
    ImGui::SliderFloat("Expose", &SPSconstant.expose, 0.f, 10.f);
    ImGui::SliderFloat("Gamma", &SPSconstant.gamma, 0.f, 10.f);

    ImGui::SliderFloat("TextureLOD", &textureLOD, 0.f, 10.f);
    ImGui::SliderFloat("HeightScale", &heightScale, 0.f, 1.f);
    ImGui::SliderFloat("bloomStrength", &SPSconstant.bloomStrength, 0.f, 1.f);
}

void RenderApp::Render(float deltaTime) {
    FLOAT color[4]{80.f / 255.f, 80.f / 255.f, 80.f / 255.f, 1.f};
    std::vector<ID3D11RenderTargetView *> rtvs{m_floatRTV.Get()};
    m_context->OMSetRenderTargets(rtvs.size(), rtvs.data(), m_DSV.Get());
    m_context->ClearRenderTargetView(m_floatRTV.Get(), color);
    m_context->ClearDepthStencilView(
        m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    /*  BaseApp::SetGlobalConsts(m_globalConstsGPU);

      Graphics::defaultSolidPSO.SetPipelineState(m_context);
      for (const auto &model : models) {
          model->Render(m_context);
      }

      Graphics::skyboxPSO.SetPipelineState(m_context);
      skybox->Render(m_context);

      Graphics::graphPSO.SetPipelineState(m_context);
      screen->Render(m_context);*/

    // Graphics::billboardPSO.SetPipelineState(m_context);
    // dotModel->Render(m_context);

    //// Mirror
    //// Stencil Buffer Masking
    // Graphics::stencliMaskPSO.SetPipelineState(m_context);
    // mirror->Render(m_context);
    //
    ////// Draw mirror
    // Graphics::reflectedSolidPSO.SetPipelineState(m_context);
    // m_context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
    // BaseApp::SetGlobalConsts(m_reflectGlobalConstsGPU);
    // for (const auto &model : models) {
    //     model->Render(m_context);
    // }
    // Graphics::reflectedSkyboxPSO.SetPipelineState(m_context);
    // skybox->Render(m_context);
    //
    //  blending
    // Graphics::blendPSO.SetPipelineState(m_context);

    /*m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);*/
    
    // Compute Shader
    Graphics::InitPSO.SetPipelineState(m_context);
    //m_context->CSSetShaderResources(0, 1, m_srvB.GetAddressOf());
    m_context->CSSetUnorderedAccessViews(0, 1, m_uavA.GetAddressOf(), NULL);
    m_context->Dispatch(m_width, m_height,
                        1);
    Utils::ComputeShaderBarrier(m_context);

    
    
    for (int i = 0; i < 2; i++) {
        Graphics::blurXPSO.SetPipelineState(m_context);
        m_context->CSSetSamplers(0, 1, Graphics::pointClampSS.GetAddressOf());
        
        m_context->CSSetShaderResources(0, 1, m_srvA.GetAddressOf());
        m_context->CSSetUnorderedAccessViews(0, 1, m_uavB.GetAddressOf(), NULL);
        m_context->Dispatch((UINT)std::ceil(m_width/32.f),
                            (UINT)std::ceil(m_height/32.f),1);
        Utils::ComputeShaderBarrier(m_context);
        
        Graphics::blurYPSO.SetPipelineState(m_context);
        m_context->CSSetShaderResources(0, 1, m_srvB.GetAddressOf());
        m_context->CSSetUnorderedAccessViews(0, 1, m_uavA.GetAddressOf(), NULL);
        m_context->Dispatch((UINT)std::ceil(m_width / 32.f),
                            (UINT)std::ceil(m_height / 32.f), 1);
        Utils::ComputeShaderBarrier(m_context);
    }

    m_context->CopyResource(m_resolvedBuffer.Get(), m_texA.Get());

    m_postProcess.Render(m_context);
}
} // namespace soku
