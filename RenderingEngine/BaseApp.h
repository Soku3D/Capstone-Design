#pragma once
#include "Timer.h"
#include "Camera.h"
namespace soku {
	class BaseApp{
	public:
		BaseApp(int width, int height);
		virtual bool Initialize();
		bool InitWindow();
		bool InitDirect3D();
		bool InitGUI();
		
		int Run();
		LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		float GetAspectRatio() const {
			return float(m_width) / (float)m_height;
		}
		
	protected:

		virtual void Render(float deltaTime) = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void UpdateGUI(float deltaTime) = 0;
	private:
		DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT m_sampleCount;
		UINT m_sampleQulity;
	protected:
		UINT m_width;
		UINT m_height;
		UINT m_guiWidth = 0;
		FLOAT m_aspectRatio;
		HWND m_hWnd;
		D3D11_VIEWPORT m_viewport;
	protected:
		void SetViewport();
		bool CreateRenderTargetView();
		bool CreateDepthBuffer();
		bool CreateRasterizerState(
			Microsoft::WRL::ComPtr <ID3D11RasterizerState>& rasterizerState,
			D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
			D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);
	protected:
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;

		Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_indexRtv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexTempTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_indexStagingTexture;

		

		Microsoft::WRL::ComPtr <ID3D11DepthStencilView> m_depthStencilView;
		Microsoft::WRL::ComPtr <ID3D11Texture2D> m_depthStencilBuffer;
		Microsoft::WRL::ComPtr <ID3D11DepthStencilState> m_depthStencilState;
		
		Microsoft::WRL::ComPtr <ID3D11RasterizerState> m_rasterizerState;
		Microsoft::WRL::ComPtr <ID3D11RasterizerState> m_rasterizerState_wireMode;

	protected:
		Timer m_timer;
		std::unique_ptr<DirectX::Mouse> mouse;
		std::shared_ptr<Camera> m_camera;
		void OnMouseMove(int delX, int delY);
		bool keyDownState[256];
		bool m_dragStartFlag = false;
		bool m_captureFlag = false;
		void CapturePNG();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_tempTexture;
		DirectX::SimpleMath::Quaternion q;
		bool m_leftButton;
		bool m_grapmode = false;
		bool m_FPSMode = true;
	};
}