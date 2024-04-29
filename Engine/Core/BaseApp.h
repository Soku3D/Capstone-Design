#pragma once
#include "Camera.h"
#include "Constants.h"
#include "GraphicsCommon.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"
#include "Timer.h"
#include "Utils.h"
#include "StructuredBuffer.h"
namespace soku {
struct Particle {
    Vector3 m_position;
    Vector3 m_color;
    Vector3 m_velocity;
    float m_time;
    float m_radius;
};
class BaseApp {
  public:
    BaseApp(int width, int height);
    virtual ~BaseApp() {}

    virtual bool Initialize();

    int Run();
    LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    float GetAspectRatio() const { return float(m_width) / (float)m_height; }

    void InitCubemaps(const std::wstring &basePath,
                      const std::wstring &filename);
    void CreateConsts();
    void UpdateGlobalConsts(const Vector3 &eyeWorld, const Matrix &viewRow,
                            const Matrix &projRow, const float &lod = 0.f,
                            const Matrix &refl = Matrix(),
                            const int &useEnv = 1, const float &delTime = 0.f);
    void SetGlobalConsts(Microsoft::WRL::ComPtr<ID3D11Buffer> &globalConstsGPU);

  protected:
    bool InitWindow();
    bool InitDirect3D();
    bool InitGUI();
    void SetViewport();
    bool CreateDepthBuffer();
    void CreateBuffers();

    virtual void Render(float deltaTime) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void UpdateGUI(float deltaTime) = 0;

  protected:
    StructuredBuffer<Particle> m_particles;
    void RenderDotBlur();
    void InitParticles(const int& count);
    void RenderParticles();
    void AddParticle(const Vector2 &position);
  private:
    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
    UINT m_sampleCount;
    UINT m_sampleQulity;
    bool useMSAA = true;

  protected:
    UINT m_width;
    UINT m_height;
    UINT m_guiWidth = 0;
    FLOAT m_aspectRatio;
    HWND m_hWnd;
    D3D11_VIEWPORT m_viewport;

  protected:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
        
    bool usebackBufferMSAA = true;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floatBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_floatRTV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_resolvedBuffer;
    // Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texA;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvA;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uavA;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texB;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvB;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uavB;

  protected:
    Timer m_timer;
    std::unique_ptr<DirectX::Mouse> mouse;
    std::shared_ptr<Camera> m_camera;
    bool keyDownState[256];
    bool m_captureFlag = false;
    bool m_cameraMove = false;
    void CapturePNG();
    void OnMouseMove(int delX, int delY);

  public:
    GlobalConstants m_globalConstsCPU;
    GlobalConstants m_reflectGlobalConstsCPU;

  protected:
    PostProcess m_postProcess;

  protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalConstsGPU;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_envSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_brdfSRV;
    protected:

};
} // namespace soku