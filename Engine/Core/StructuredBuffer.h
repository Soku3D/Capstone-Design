#pragma once
#include "Utils.h"

namespace soku {
template <typename T> class StructuredBuffer {
  public:
    void Initilaize(Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        Utils::CreateStructuredBuffer(device, m_cpu, m_gpu, m_srv, m_uav,
                                      m_rtv);
        Utils::CreateStagingBuffer(m_cpu, m_staging, device);
    }
    std::vector<T> m_cpu;
  private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_gpu;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_staging;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
};
} // namespace soku