#include "pch.h"
#include "BaseApp.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
namespace soku {
BaseApp *appPtr = nullptr;
LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return appPtr->wndProc(hWnd, msg, wParam, lParam);
}
BaseApp::BaseApp(int width, int height)
    : m_width(width), m_height(height), m_timer(Timer()),
      m_aspectRatio((float)width / height), m_viewport(D3D11_VIEWPORT()),
      m_hWnd(nullptr), m_sampleQulity(0), m_sampleCount(4) {
    appPtr = this;
    mouse = std::make_unique<DirectX::Mouse>();
    m_camera = std::make_shared<Camera>();
}
bool BaseApp::Initialize() {
    if (!InitWindow()) {
        return false;
    }
    mouse->SetWindow(m_hWnd);
    if (!InitDirect3D()) {
        return false;
    }
    if (!InitGUI()) {
        return false;
    }
    return true;
}
void BaseApp::InitCubemaps(const std::wstring &basePath,
                           const std::wstring &filename) {
    std::wstring brdfPath = basePath + filename + L"Brdf.dds";
    std::wstring irradiancePath = basePath + filename + L"DiffuseHDR.dds";
    std::wstring envPath = basePath + filename + L"EnvHDR.dds";
    std::wstring specularPath = basePath + filename + L"SpecularHDR.dds";

    Utils::CreateDDSTexture(envPath, m_envSRV, m_device, true);
    Utils::CreateDDSTexture(irradiancePath, m_irradianceSRV, m_device, true);
    Utils::CreateDDSTexture(specularPath, m_specularSRV, m_device, true);
    Utils::CreateDDSTexture(brdfPath, m_brdfSRV, m_device, false);

    std::vector<ID3D11ShaderResourceView *> srvs = {
        m_envSRV.Get(), m_irradianceSRV.Get(), m_specularSRV.Get(),
        m_brdfSRV.Get()};
    m_context->PSSetShaderResources((UINT)10, (UINT)srvs.size(), srvs.data());
}
void BaseApp::CreateConsts() {
    Utils::CreateConstantBuffer(m_globalConstsCPU, m_globalConstsGPU, m_device);
    Utils::CreateConstantBuffer(m_reflectGlobalConstsCPU,
                                m_reflectGlobalConstsGPU, m_device);
}
void BaseApp::UpdateGlobalConsts(const Vector3 &eyeWorld, const Matrix &viewRow,
                                 const Matrix &projRow, const float &lod,
                                 const Matrix &refl, const int &useEnv,
                                 const float &delTime) {
    m_globalConstsCPU.eyePos = eyeWorld;
    m_globalConstsCPU.view = viewRow.Transpose();
    m_globalConstsCPU.proj = projRow.Transpose();
    m_globalConstsCPU.viewProj = viewRow * projRow;
    m_globalConstsCPU.viewProj = m_globalConstsCPU.viewProj.Transpose();
    m_globalConstsCPU.lod = lod;
    m_globalConstsCPU.useEnv = useEnv;
    m_globalConstsCPU.time += delTime;
    // std::cout << m_globalConstsCPU.time << '\n';
    m_reflectGlobalConstsCPU = m_globalConstsCPU;
    m_reflectGlobalConstsCPU.view = refl * viewRow;
    m_reflectGlobalConstsCPU.view = m_reflectGlobalConstsCPU.view.Transpose();
    m_reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow);
    m_reflectGlobalConstsCPU.viewProj =
        m_reflectGlobalConstsCPU.viewProj.Transpose();

    Utils::UpdateConstantBuffer(m_reflectGlobalConstsCPU,
                                m_reflectGlobalConstsGPU, m_context);
    Utils::UpdateConstantBuffer(m_globalConstsCPU, m_globalConstsGPU,
                                m_context);
}
void BaseApp::SetGlobalConsts(
    Microsoft::WRL::ComPtr<ID3D11Buffer> &globalConstsGPU) {
    m_context->VSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
    m_context->CSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
    m_context->PSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
    m_context->GSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
}
bool BaseApp::InitWindow() {
    // Create WindowClass
    WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                     CS_CLASSDC,
                     MainProc,
                     0L,
                     0L,
                     GetModuleHandle(NULL),
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     L"HongLabGraphics", // lpszClassName, L-string
                     NULL};

    if (!RegisterClassEx(&wc)) {
        std::cout << "RegisterClassEx() failed." << std::endl;
        return false;
    }
    RECT rect = {(LONG)0, (LONG)0, (LONG)m_width, (LONG)m_height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    // Create Window
    m_hWnd = CreateWindow(wc.lpszClassName, L"Rendering Engine",
                          WS_OVERLAPPEDWINDOW,
                          100, 
                          100, 
                          rect.right - rect.left, 
                          rect.bottom - rect.top, 
                          NULL, NULL, wc.hInstance, NULL);
    if (!m_hWnd) {
        return false;
    }
    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02; // 마우스 장치
    Rid[0].dwFlags = 0;
    Rid[0].hwndTarget = m_hWnd;

    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    return true;
}
bool BaseApp::InitDirect3D() {
    using namespace Microsoft::WRL;

    UINT createDeviceFlag = 0;
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0,
                                         D3D_FEATURE_LEVEL_10_0};
    D3D_FEATURE_LEVEL featureLevel;
#if defined(DEBUG) || (_DEBUG)
    createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferDesc.Width = m_width;
    scDesc.BufferDesc.Height = m_height;
    scDesc.BufferDesc.Format = m_backBufferFormat;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferCount = 2;
    scDesc.BufferUsage =
        DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    scDesc.OutputWindow = m_hWnd;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlag,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &scDesc,
        m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel,
        m_context.GetAddressOf());
    if (FAILED(hr)) {
        std::cout << "D3D11CreateDeviceAndSwapChain Failed" << '\n';
        return false;
    }
    Graphics::InitCommonStates(m_device);

    CreateBuffers();
    SetViewport();

    CreateConsts();
    return true;
}
void BaseApp::SetViewport() {
    m_viewport = {0.f, 0.f, (float)m_width, (float)m_height, 0.0f, 1.0f};
    m_context->RSSetViewports(1, &m_viewport);
}
bool BaseApp::CreateDepthBuffer() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    D3D11_TEXTURE2D_DESC depthDesc;
    ZeroMemory(&depthDesc, sizeof(depthDesc));
    depthDesc.Width = m_width;
    depthDesc.Height = m_height;
    depthDesc.MipLevels = depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = (m_sampleQulity > 0) ? 4 : 1;
    depthDesc.SampleDesc.Quality =
        (m_sampleQulity > 0) ? m_sampleQulity - 1 : 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = m_device->CreateTexture2D(&depthDesc, nullptr,
                                           depthBuffer.ReleaseAndGetAddressOf());

    if (FAILED(hr)) {
        std::cout << "CreateDepthBuffer Failed" << '\n';
        return false;
    }
    hr = m_device->CreateDepthStencilView(depthBuffer.Get(), 0,
                                          m_DSV.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        std::cout << "CreateDepthStencilView Failed" << '\n';
        return false;
    }
    return true;
}
void BaseApp::CreateBuffers() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    
    renderTex.Initialize(m_width, m_height, BaseApp::m_backBufferFormat,
                         m_device);
    ThrowIfFailed(m_device->CreateRenderTargetView(
        backBuffer.Get(), nullptr, m_backBufferRTV.ReleaseAndGetAddressOf()));

    D3D11_TEXTURE2D_DESC texDesc;
    backBuffer->GetDesc(&texDesc);

    m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4,
                                            &m_sampleQulity);

    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE |
                        D3D11_BIND_UNORDERED_ACCESS;

    ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL, m_resolvedBuffer.ReleaseAndGetAddressOf()));
    ThrowIfFailed(m_device->CreateShaderResourceView(
        m_resolvedBuffer.Get(), NULL, m_resolvedSRV.ReleaseAndGetAddressOf()));

    texDesc.SampleDesc.Count = (m_sampleQulity > 0) ? 4 : 1;
    texDesc.SampleDesc.Quality = (m_sampleQulity > 0) ? m_sampleQulity - 1 : 0;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_device->CreateTexture2D(&texDesc, NULL,
                              m_floatBuffer.ReleaseAndGetAddressOf());
    m_device->CreateRenderTargetView(m_floatBuffer.Get(), NULL,
                                     m_floatRTV.ReleaseAndGetAddressOf());

    CreateDepthBuffer();

    backBuffer->GetDesc(&texDesc);
    texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE |
                        D3D11_BIND_UNORDERED_ACCESS;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Format = texDesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL,
                                            m_texA.ReleaseAndGetAddressOf()));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_texA.Get(), NULL, m_srvA.ReleaseAndGetAddressOf()));
    ThrowIfFailed(m_device->CreateUnorderedAccessView(m_texA.Get(), NULL, m_uavA.ReleaseAndGetAddressOf()));

    ThrowIfFailed(m_device->CreateTexture2D(&texDesc, NULL,
                                            m_texB.ReleaseAndGetAddressOf()));
    ThrowIfFailed(m_device->CreateShaderResourceView(m_texB.Get(), NULL, m_srvB.ReleaseAndGetAddressOf()));
    ThrowIfFailed(m_device->CreateUnorderedAccessView(m_texB.Get(), NULL, m_uavB.ReleaseAndGetAddressOf()));
    m_postProcess.Initialize(m_device, m_context, {m_resolvedSRV},
                             {m_backBufferRTV}, m_width, m_height);
}

bool BaseApp::InitGUI() {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsLight();
    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());

    return true;
}
int BaseApp::Run() {
    MSG msg = {};
    m_timer.Start();
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        } else {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("GUI");
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            UpdateGUI(0.f);
            // ImGui::SetWindowPos({ 0.f,0.f });
            // m_guiWidth = (UINT)ImGui::GetWindowSize().x;
            // ImGui::SetWindowSize(ImVec2(m_guiWidth, m_height));
            ImGui::End();
            ImGui::Render();

            if (m_captureFlag) {
                m_captureFlag = false;
                CapturePNG();
            }

            Update(ImGui::GetIO().DeltaTime);
            Render(ImGui::GetIO().DeltaTime);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            m_swapChain->Present(1, 0);
            OnMouseMove(0, 0);
        }
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    DestroyWindow(m_hWnd);
    return int(msg.wParam);
}
void BaseApp::OnMouseMove(int delX, int delY) {
    m_camera->UpdateMouse(delX, delY);
}
void BaseApp::CapturePNG() {
    // Utils::SavePNG(img, desc.Width, desc.Height);
}
LRESULT BaseApp::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (m_swapChain) { // 처음 실행이 아닌지 확인

            m_width = int(LOWORD(lParam));
            m_height = int(HIWORD(lParam));
            m_guiWidth = 0;

            m_backBufferRTV.Reset();
            m_swapChain->ResizeBuffers(0, // 현재 개수 유지
                                       (UINT)LOWORD(lParam), // 해상도 변경
                                       (UINT)HIWORD(lParam),
                                       DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
                                       0);
            CreateBuffers();
            SetViewport();

            // 화면 해상도가 바뀌면 카메라의 aspect ratio도 변경
            m_camera->SetAspectRatio(this->GetAspectRatio());
            std::cout << m_width << " " << m_height << '\n';
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        keyDownState[wParam] = true;
        /*if (wParam == 'V' && !m_FPSMode)
            m_FPSMode = true;
        else if (wParam == 'V' && m_FPSMode)
            m_FPSMode = false;
        if (wParam == 'G' && !m_grapmode)
            m_grapmode = true;
        else if (wParam == 'G' && m_grapmode)
            m_grapmode = false;*/
        break;
    case WM_KEYUP:
        keyDownState[wParam] = false;
        if (int(wParam) == int('C')) {
            m_captureFlag = true;
        }
        if (int(wParam) == int('F')) {
            m_cameraMove = !m_cameraMove;
        }
        break;
    //case WM_INPUT:
    //    if (m_cameraMove) {
    //        RAWINPUT raw;
    //        UINT rawSize = sizeof(raw);

    //        const UINT resultData =
    //            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
    //                            &raw, &rawSize, sizeof(RAWINPUTHEADER));
    //        // if (raw.header.dwType == RIM_TYPEMOUSE && m_FPSMode) {
    //        int deltaX = raw.data.mouse.lLastX;
    //        int deltaY = raw.data.mouse.lLastY;
    //        // std::cout << deltaX << " " << deltaY << '\n';
    //        OnMouseMove(deltaX, deltaY);
    //        DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
    //        break;
    //    }
    case WM_LBUTTONDOWN:
        if (!lButtionDown)
            lButtonDrag = true;
        lButtionDown = true;
        break;
    case WM_LBUTTONUP:
        lButtonDrag = false;
        lButtionDown = false;
        break;
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_MOUSEMOVE:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
void BaseApp::RenderDotBlur() {
    // Compute Shader
    Graphics::InitPSO.SetPipelineState(m_context);
    // m_context->CSSetShaderResources(0, 1, m_srvB.GetAddressOf());
    m_context->CSSetUnorderedAccessViews(0, 1, m_uavA.GetAddressOf(), NULL);
    m_context->Dispatch(m_width, m_height, 1);
    Utils::ComputeShaderBarrier(m_context);

    for (int i = 0; i < 1000; i++) {
        m_context->CSSetSamplers(0, 1, Graphics::pointClampSS.GetAddressOf());
        Graphics::blurXGroupCachePSO.SetPipelineState(m_context);

        m_context->CSSetShaderResources(0, 1, m_srvA.GetAddressOf());
        m_context->CSSetUnorderedAccessViews(0, 1, m_uavB.GetAddressOf(), NULL);
        m_context->Dispatch((UINT)std::ceil(m_width / 256.f), m_height, 1);
        Utils::ComputeShaderBarrier(m_context);

        Graphics::blurYGroupCachePSO.SetPipelineState(m_context);
        m_context->CSSetShaderResources(0, 1, m_srvB.GetAddressOf());
        m_context->CSSetUnorderedAccessViews(0, 1, m_uavA.GetAddressOf(), NULL);
        m_context->Dispatch(m_width, (UINT)std::ceil(m_height / 256.f), 1);
        Utils::ComputeShaderBarrier(m_context);
    }

    // exit(-1);

    m_context->CopyResource(m_resolvedBuffer.Get(), m_texA.Get());
}

void BaseApp::InitParticles(const int &count) {

    m_particles.m_cpu.resize(count);
    
    std::mt19937 gen(0);
    std::uniform_real_distribution<float> urd_p(-1.f, 1.f);
    std::uniform_real_distribution<float> urd_c(0.f, 1.f);
    std::uniform_real_distribution<float> urd_r(0.01f, 0.03f);
    std::uniform_real_distribution<float> urd_v(-1.f, -0.05f);
    std::uniform_real_distribution<float> urd_t(0.5f, 1.5f);

    for (auto &p : m_particles.m_cpu) {
        //p.m_position = Vector3(urd_p(gen), urd_p(gen), 0.f);
        p.m_position = Vector3(0.f,0.f, 0.f);
        p.m_color = Vector3(urd_c(gen), urd_c(gen), urd_c(gen));
        p.time = -1.f;
        p.width = urd_r(gen);
        p.m_velocity = Vector3(urd_v(gen), 0.f, 0.f);
    }

    m_particles.Initilaize(m_device);
}

void BaseApp::RenderParticles() {
    Graphics::drawingParticlesBlendPSO.SetPipelineState(m_context);
    m_context->CSSetShader(NULL, NULL, 0);
    m_context->VSSetShaderResources(0, 1, m_particles.GetSrvAddressOf());
    m_context->Draw(m_particles.m_cpu.size(), 0);
    ID3D11ShaderResourceView *nullSRVs[1] = {0};
    m_context->VSSetShaderResources(0, 1, nullSRVs);
}
void BaseApp::UpdateParticles() {
    Graphics::updateParticlePSO.SetPipelineState(m_context);
    m_context->CSSetUnorderedAccessViews(0, 1, m_particles.GetUavAddressOf(),NULL);
    m_context->Dispatch((UINT)(std::ceil(m_particles.m_cpu.size() / 256.f)), 1.f, 1.f);
    //m_context->CSSetShaderResources()
    //m_context->CSSetConstantBuffers()
    Utils::ComputeShaderBarrier(m_context);
}
void BaseApp::DisspationParticles() {
    Graphics::disspateParticlePSO.SetPipelineState(m_context);
    m_context->CSSetUnorderedAccessViews(0, 1,
                                         renderTex.GetUavAddressOf(),
                                         NULL);
    m_context->Dispatch((UINT)(std::ceil(m_width)),
                        (UINT)(std::ceil(m_height)),1.f);
    Utils::ComputeShaderBarrier(m_context);
}
} // namespace soku
