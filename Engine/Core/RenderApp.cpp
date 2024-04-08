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
    auto boxData = GeometryGenerator::MakeBox();
    auto grid = std::make_shared<Model>();
    grid->Initialize({boxData}, m_device);
    models.push_back(grid);

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

    BaseApp::UpdateGlobalConsts(eyeWorld, viewRow, projRow);
    for (const auto &model : models) {
        model->m_meshConstantsCPU.world =
            Matrix::CreateTranslation(Vector3(0.f, 0.f, 3.f));
        model->m_meshConstantsCPU.world =
            model->m_meshConstantsCPU.world.Transpose();
        Utils::UpdateConstantBuffer(model->m_meshConstantsCPU,
                                    model->m_meshConstantsGPU, m_context);
    }
}
void RenderApp::UpdateGUI(float deltaTime) {}

void RenderApp::Render(float deltaTime) {
    FLOAT color[4]{0.f, 0.f, 0.f, 1.f};
    m_context->OMSetRenderTargets(1, m_floatRTV.GetAddressOf(), m_DSV.Get());
    m_context->ClearRenderTargetView(m_floatRTV.Get(), color);
    m_context->ClearDepthStencilView(
        m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    BaseApp::SetGlobalConsts();

    Graphics::defaultSolidPSO.SetPipelineState(m_context);

    for (const auto &model : models) {
        model->Render(m_context);
    }

    m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);
    Graphics::combinePSO.SetPipelineState(m_context);

    m_postProcess.Render(m_context);
}
} // namespace soku
