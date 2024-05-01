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
    BaseApp::InitParticles(2560);
    renderTex.Initialize(m_width, m_height, BaseApp::m_backBufferFormat,
                         m_device);
    return true;
}
void RenderApp::Update(float dt) {
    // std::cout << mouse->GetState().x << ' ' << mouse->GetState().y << '\n';
    // BaseApp::UpdateParticles();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> urd(-0.5f, 0.5f);
    std::uniform_real_distribution<float> randomVel(1.f,3.f);
    std::uniform_real_distribution<float> randomTheta(-3.14f, 3.14f);
    std::uniform_real_distribution<float> randomV(0.0f, 0.03f);
    std::uniform_real_distribution<float> randomW(0.01f, 0.05f);
    std::uniform_real_distribution<float> randomLife(0.01f, 1.f);
    std::uniform_real_distribution<float> randomCoefficient(0.5f, 0.8f);
    int count = 50;
    float g = -9.8f;
    for (auto &p : m_particles.m_cpu) {
        auto state = mouse->GetState();
        if (lButtionDown && p.time < 0 && count > 0) {
            const float theta = randomTheta(gen);
            float x = (2.f * state.x / m_width) - 1.f;
            float y = -(2.f * state.y / m_height) + 1.f;
            p.m_position = Vector3(x, y, 0.f);
            p.m_velocity = Vector3(cos(theta), sin(theta), 0.f) * 2.f;
            p.time = randomLife(gen) * 1.5f;
            p.width = randomW(gen);
            count--;
        }
    }
    count = 10;
    for (auto &p : m_particles.m_cpu) {
        if (p.time < 0 && count > 0) {
            const float theta = randomTheta(gen);
            p.m_position = Vector3(cos(theta), sin(theta), 0.f) *
                           randomV(gen);
            p.m_velocity = Vector3(-1.f, 0.f, 0.f) * randomVel(gen);
            p.time = randomLife(gen) * 1.5f;
            p.width = randomW(gen);
            count--;
        }
    }
    for (auto &p : m_particles.m_cpu) {
        if (p.time < 0.0f)
            continue;
        p.time -= dt;
        p.m_velocity = p.m_velocity + Vector3(0.f, 1.f, 0.f) * g * dt;
        p.m_position += p.m_velocity * dt;

        float x = p.m_position.x;
        float y = p.m_position.y;

        if (y < -0.8f) {
            p.m_velocity.y *= -randomCoefficient(gen);
        }
        if (x < -0.8f) {
            p.m_velocity.x *= -randomCoefficient(gen);
        }
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
