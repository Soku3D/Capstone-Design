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
    return true;
}
void RenderApp::Update(float deltaTime) {
    //std::cout << mouse->GetState().x << ' ' << mouse->GetState().y << '\n';
}
void RenderApp::UpdateGUI(float deltaTime) {
    ImGui::RadioButton("test", m_captureFlag);
}

void RenderApp::Render(float deltaTime) { 
    BaseApp::RenderParticles();   
}


} // namespace soku
