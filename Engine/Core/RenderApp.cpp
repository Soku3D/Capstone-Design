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
    BaseApp::MakeParticles(1);
    testTex.Initialize(m_width, m_height, DXGI_FORMAT_R16G16B16A16_FLOAT,
                       m_device);
    return true;
}
void RenderApp::Update(float deltaTime) {
        
}
void RenderApp::UpdateGUI(float deltaTime) {
    ImGui::RadioButton("test", m_captureFlag);
}

void RenderApp::Render(float deltaTime) {
    
    BaseApp::UpdateParticles();
    DrawSprites();
    const float blendFactor[4] = {1.f, 1.f, 1.f, 1.f};
    m_context->OMSetBlendState(Graphics::addBS.Get(), blendFactor, 0xffffffff);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_context->CopyResource(backBuffer.Get(), testTex.GetTex());
   
}
void RenderApp::DrawSprites() {


    BaseApp::SetViewport();
    m_context->OMSetRenderTargets(1, testTex.GetAddressOfRTV(), NULL);
    Graphics::drawingParticlesBlendPSO.SetPipelineState(m_context);

    m_context->VSSetShaderResources(0, 1, particles.m_srv.GetAddressOf());
    m_context->Draw(UINT(particles.m_cpu.size()), 0);
   
}

} // namespace soku
