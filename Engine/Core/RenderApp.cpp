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
    };
    BaseApp::InitParticles(256);
    renderTex.Initialize(m_width, m_height, BaseApp::m_backBufferFormat,
                         m_device);
    return true;
}
void RenderApp::Update(float deltaTime) {
    // std::cout << mouse->GetState().x << ' ' << mouse->GetState().y << '\n';
    BaseApp::UpdateParticles();
}
void RenderApp::UpdateGUI(float deltaTime) {
    ImGui::RadioButton("test", m_captureFlag);
}

void RenderApp::Render(float deltaTime) { RenderSpinParticles(); }
void RenderApp::RenderSpinParticles() {
    m_context->OMSetRenderTargets(0, NULL, NULL);
    DisspationParticles();
    m_context->OMSetRenderTargets(1, renderTex.GetRtvAddressOf(), NULL);
    BaseApp::RenderParticles();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_context->CopyResource(backBuffer.Get(), renderTex.GetTex());
}
    
} // namespace soku
