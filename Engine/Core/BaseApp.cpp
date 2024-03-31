#include "pch.h"
#include "Utils.h"
#include "BaseApp.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace soku
{
	BaseApp* appPtr = nullptr;
	LRESULT WINAPI MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		return appPtr->wndProc(hWnd, msg, wParam, lParam);
	}
	BaseApp::BaseApp(int width, int height) :
		m_width(width),
		m_height(height),
		m_timer(Timer()),
		m_aspectRatio((float)width / height),
		m_viewport(D3D11_VIEWPORT()),
		m_hWnd(nullptr),
		m_sampleQulity(0),
		m_sampleCount(4)
	{
		appPtr = this;
		mouse = std::make_unique<DirectX::Mouse>();
		m_camera = std::make_shared<Camera>();
	}
	bool BaseApp::Initialize()
	{
		if (!InitWindow())
		{
			return false;
		}
		mouse->SetWindow(m_hWnd);
		if (!InitDirect3D())
		{
			return false;
		}
		if (!InitGUI())
		{
			return false;
		}
		return true;
	}
	bool BaseApp::InitWindow()
	{
		// Create WindowClass
		WNDCLASSEXW wc = {
			sizeof(wc),
			CS_CLASSDC,
			MainProc,
			0,0,
			GetModuleHandle(NULL),
			0,0,0,0,
			L"class",
			0 };
		RegisterClassExW(&wc);
		RECT rect = { 0,0,m_width, m_height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

		// Create Window
		m_hWnd = CreateWindowW(L"class", L"wnd", WS_OVERLAPPEDWINDOW,
			10, 10,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL, NULL, wc.hInstance, NULL
		);
		if (!m_hWnd) {
			return false;
		}
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;  // 마우스 장치
		Rid[0].dwFlags = 0;
		Rid[0].hwndTarget = m_hWnd;

		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);

		return true;
	}
	bool BaseApp::InitDirect3D()
	{
		using namespace Microsoft::WRL;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;

		UINT createDeviceFlag = 0;
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_0
		};
		D3D_FEATURE_LEVEL featureLevel;
#if defined(DEBUG) || (_DEBUG)
		createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlag,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&featureLevel,
			context.GetAddressOf()))) {
			MessageBox(NULL, "CreateDevice Failed", NULL, 0);
			return false;
		}

		device->CheckMultisampleQualityLevels(
			swapChainFormat,
			m_sampleCount,
			&m_sampleQulity);

		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(scDesc));
		scDesc.BufferDesc.Width = m_width;
		scDesc.BufferDesc.Height = m_height;
		scDesc.BufferDesc.Format = swapChainFormat;
		scDesc.BufferDesc.RefreshRate.Numerator = 60;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
		scDesc.SampleDesc.Count = (m_sampleQulity > 0) ? m_sampleCount : 1;
		scDesc.SampleDesc.Quality = (m_sampleQulity > 0) ? m_sampleQulity - 1 : 0;
		scDesc.BufferCount = 2;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		scDesc.OutputWindow = m_hWnd;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.Windowed = true;

		HRESULT hr =
			D3D11CreateDeviceAndSwapChain(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				createDeviceFlag,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&scDesc,
				m_swapChain.GetAddressOf(),
				m_device.GetAddressOf(),
				&featureLevel,
				m_context.GetAddressOf()
			);
		if (FAILED(hr)) {
			std::cout << "D3D11CreateDeviceAndSwapChain Failed" << '\n';
			return false;
		}
		if (!CreateRenderTargetView())
		{
			std::cout << "CreateRenderTargetView Failed" << '\n';
			return false;
		}
		SetViewport();
		if (!CreateDepthBuffer()) {
			return false;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask =
			D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc =
			D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		if (FAILED(m_device->CreateDepthStencilState(
			&depthStencilDesc, m_depthStencilState.GetAddressOf()))) {
			std::cout << "CreateDepthStencilState() failed." << std::endl;
		}

		if (!CreateRasterizerState(m_rasterizerState_wireMode, D3D11_FILL_MODE::D3D11_FILL_WIREFRAME,
			D3D11_CULL_NONE)) {
			return false;
		}
		if (!CreateRasterizerState(m_rasterizerState)) {
			return false;
		}


		return true;
	}
	bool BaseApp::CreateRenderTargetView() {
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		if (backBuffer)
		{
			if (FAILED(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, 
				m_renderTargetView.GetAddressOf())))
			{
				std::cout << "CreateRenderTargetView(backbuffer) FAILED\n";
				return false;
			}
			D3D11_TEXTURE2D_DESC desc;
			backBuffer->GetDesc(&desc);
			if (FAILED(m_device->CreateTexture2D(&desc, 0, m_indexTexture.GetAddressOf()))) {
				std::cout << "CreateTexture2D(indexTexture) FAILED\n";
				return false;
			}
			if (FAILED(m_device->CreateRenderTargetView(m_indexTexture.Get(), nullptr,
				m_indexRtv.GetAddressOf())))
			{
				std::cout << "CreateRenderTargetView(indexRtv) FAILED\n";
				return false;
			}
			backBuffer->GetDesc(&desc);
			desc.SampleDesc = { 1,0 };
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = 0;
			if (FAILED(m_device->CreateTexture2D(&desc, 0, m_tempTexture.GetAddressOf()))) {
				std::cout << "CreateTexture2D(m_tempTexture) FAILED\n";
				return false;
			}
			if (FAILED(m_device->CreateTexture2D(&desc, 0, m_indexTempTexture.GetAddressOf()))) {
				std::cout << "CreateTexture2D(m_indexTempTexture) FAILED\n";
				return false;
			}
			
			desc.Width = 1;
			desc.Height = 1;
			desc.SampleDesc = { 1,0 };
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			if (FAILED(m_device->CreateTexture2D(&desc, 0, m_indexStagingTexture.GetAddressOf()))) {
				std::cout << "CreateTexture2D(m_indexStagingTexture) FAILED\n";
				return false;
			}
			return true;
		}
		return false;
	}
	void BaseApp::SetViewport() {
		m_viewport =
		{
			0.f,0.f,
			(float)m_width,
			(float)m_height,
			0.0f,1.0f
		};
		m_context->RSSetViewports(1, &m_viewport);
	}
	bool BaseApp::CreateDepthBuffer() {
		Microsoft::WRL::ComPtr <ID3D11Texture2D> depthBuffer;
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = m_width;
		depthDesc.Height = m_height;
		depthDesc.MipLevels = depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = (m_sampleQulity > 0) ? m_sampleCount : 1;
		depthDesc.SampleDesc.Quality = (m_sampleQulity > 0) ? m_sampleQulity - 1 : 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		HRESULT hr = m_device->CreateTexture2D(&depthDesc, nullptr, depthBuffer.GetAddressOf());

		if (FAILED(hr)) {
			std::cout << "CreateDepthBuffer Failed" << '\n';
			return false;
		}
		hr = m_device->CreateDepthStencilView(depthBuffer.Get(), 0, m_depthStencilView.GetAddressOf());
		if (FAILED(hr)) {
			std::cout << "CreateDepthStencilView Failed" << '\n';
			return false;
		}
		return true;
	}
	bool BaseApp::CreateRasterizerState(Microsoft::WRL::ComPtr <ID3D11RasterizerState>& rasterizerState,
		D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode) {
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.CullMode = cullMode;
		rsDesc.FillMode = fillMode;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthClipEnable = true;

		HRESULT hr =
			m_device->CreateRasterizerState(&rsDesc, rasterizerState.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			std::cout << "CreateRasterizerState Failed\n";
			return false;
		}
		//m_context->RSSetState(rasterizerState.Get());

		return true;
	}

	bool BaseApp::InitGUI()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsLight();
		ImGui_ImplWin32_Init(m_hWnd);
		ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());

		return true;
	}
	int BaseApp::Run()
	{
		MSG msg = {  };
		m_timer.Start();
		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
			else {
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImGui::Begin("Hello, world!");

				UpdateGUI(0.f);
				//ImGui::SetWindowPos({ 0.f,0.f });
				//m_guiWidth = (UINT)ImGui::GetWindowSize().x;
				//ImGui::SetWindowSize(ImVec2(m_guiWidth, m_height));
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
	void  BaseApp::OnMouseMove(int delX, int delY) {
		m_camera->UpdateMouse(delX, delY);
	}
	void BaseApp::CapturePNG()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backbuffer.GetAddressOf()));
		m_context->ResolveSubresource(m_tempTexture.Get(), 0, backbuffer.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

		D3D11_TEXTURE2D_DESC desc;
		m_tempTexture->GetDesc(&desc);
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
		if (FAILED(m_device->CreateTexture2D(&desc, nullptr,
			stagingTexture.GetAddressOf()))) {
			std::cout << "CreateTexture2D Failed()" << std::endl;
		}
		D3D11_BOX stagingbox{
		0,0,
		0,
		m_width, m_height
		,1
		};
		m_context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0,
			m_tempTexture.Get(), 0, &stagingbox);

		std::vector<unsigned char> img(desc.Width * desc.Height * 4);

		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &ms);
		uint8_t* pData = (uint8_t*)ms.pData;
		for (int h = 0; h < desc.Height; h++)
		{
			memcpy(&img[h * desc.Width * 4], &pData[h * ms.RowPitch],
				desc.Width * sizeof(uint8_t) * 4);
		}
		m_context->Unmap(stagingTexture.Get(), 0);

		Utils::SavePNG(img, desc.Width, desc.Height);
	}
	LRESULT BaseApp::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (m_swapChain) { // 처음 실행이 아닌지 확인

				m_width = int(LOWORD(lParam));
				m_height = int(HIWORD(lParam));
				m_guiWidth = 0;

				m_renderTargetView.Reset();
				m_swapChain->ResizeBuffers(0, // 현재 개수 유지
					(UINT)LOWORD(lParam), // 해상도 변경
					(UINT)HIWORD(lParam),
					DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
					0);
				CreateRenderTargetView();
				CreateDepthBuffer();
				SetViewport();

				// 화면 해상도가 바뀌면 카메라의 aspect ratio도 변경
				m_camera->SetAspectRatio(this->GetAspectRatio());
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			keyDownState[wParam] = true;
			if (wParam == 'V' && !m_FPSMode)
				m_FPSMode = true;
			else if (wParam == 'V' && m_FPSMode)
				m_FPSMode = false;
			if (wParam == 'G' && !m_grapmode)
				m_grapmode = true;
			else if (wParam == 'G' && m_grapmode)
				m_grapmode = false;
			break;
		case WM_KEYUP:
			keyDownState[wParam] = false;
			if (int(wParam) == int('C')) {
				m_captureFlag = true;
			}
			break;

		case WM_INPUT:
		{
			RAWINPUT raw;
			UINT rawSize = sizeof(raw);

			const UINT resultData = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));
			if (raw.header.dwType == RIM_TYPEMOUSE && m_FPSMode) {
				int deltaX = raw.data.mouse.lLastX;
				int deltaY = raw.data.mouse.lLastY;
				//std::cout << deltaX << " " << deltaY << '\n';
				OnMouseMove(deltaX, deltaY);
			}
			DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN:
			if (!mouse->GetState().leftButton)
				m_dragStartFlag = true; 
		case WM_LBUTTONUP:
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
}

