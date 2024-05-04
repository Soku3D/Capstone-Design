#include "pch.h"
#include "Sph.h"
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
    };
    
    m_sphSimulator = std::make_shared<Sph>();
    m_sphSimulator->Initialize(m_device, 2048);
    return true;
}
void RenderApp::Update(float dt) { 
    m_sphSimulator->Update(m_context, dt, GetAspectRatio());
}
void RenderApp::UpdateGUI(float deltaTime) {
    ImGui::RadioButton("test", m_captureFlag);
}

void RenderApp::Render(float deltaTime) {
    m_context->OMSetRenderTargets(0, NULL, NULL);
    // DisspationParticles();
    m_context->OMSetRenderTargets(1, renderTex.GetRtvAddressOf(), NULL);
    FLOAT black[4] = {0.f, 0.f, 0.f, 1.f};
    m_context->ClearRenderTargetView(renderTex.GetRtv(), black);

    m_sphSimulator->Render(m_context);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_context->CopyResource(backBuffer.Get(), renderTex.GetTex());
}
} // namespace soku
