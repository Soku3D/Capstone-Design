#pragma once
#include "Utils.h"

namespace soku {
template <typename T> class StructuredBuffer {
  public:
    void Initilaize(Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        Utils::CreateStructuredBuffer(device, UINT(m_cpu.size()),
                                           sizeof(T), m_cpu.data(),
                                           m_gpu, m_srv, m_uav);
        Utils::CreateStagingBuffer(m_cpu, m_staging, device);
    }
    ID3D11ShaderResourceView **GetSrvAddressOf() {
        return m_srv.GetAddressOf();
    }
    ID3D11UnorderedAccessView **GetUavAddressOf() {
        return m_uav.GetAddressOf();
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