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
    BaseApp::InitParticles(50);
    renderTex.Initialize(m_width, m_height, BaseApp::m_backBufferFormat,
                         m_device);
    return true;
}
void RenderApp::Update(float dt) {
    // std::cout << mouse->GetState().x << ' ' << mouse->GetState().y << '\n';
    // BaseApp::UpdateParticles();
    std::mt19937 gen(dt);
    std::uniform_real_distribution<float> urd(-0.5f, 0.5f);
   
    float g = 9.8f;
    for (auto &p : m_particles.m_cpu) {
        p.time -= dt;
        auto state = mouse->GetState();
        if (lButtionDown && !lButtonDrag) {
            float x = (2.f * state.x / m_width) - 1.f;
            float y = -(2.f * state.y / m_height) + 1.f;
            p.m_position = Vector3(x, y, 0.f);
            p.m_velocity = Vector3(-1.f, urd(gen), 0.f);
        }
        if (p.time <= 0) {
            p.time = 1.f;
            p.m_position = Vector3(0.f, 0.f, 0.f);
            p.m_velocity = Vector3(-1.f, urd(gen), 0.f);
        }
        p.m_velocity = p.m_velocity - Vector3(0.f, 1.f, 0.f)* g * dt;
        p.m_position = p.m_position + p.m_velocity * dt - Vector3(0.f,1.f,0.f)* 0.5f * 9.8f * dt * dt;
    }
    D3D11_MAPPED_SUBRESOURCE ms;
    m_context->Map(m_particles.GetStaging(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    memcpy(ms.pData, m_particles.m_cpu.data(),
           sizeof(Particle) * m_particles.m_cpu.size());
    m_context->Unmap(m_particles.GetStaging(), 0);

    m_context->CopyResource(m_particles.GetGpu(), m_particles.GetStaging());
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
    BaseApp::RenderParticles();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_context->CopyResource(backBuffer.Get(), renderTex.GetTex());
}
} // namespace soku
