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
    BaseApp::MakeParticles(256);

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
    
}
void RenderApp::UpdateGUI(float deltaTime) {
   
}

void RenderApp::Render(float deltaTime) {
    
}

} // namespace soku
