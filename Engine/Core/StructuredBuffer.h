#pragma once
#include "Utils.h"

namespace soku {
template <typename T> class StructuredBuffer {
  public:
    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device) {
        Utils::CreateStructuredBuffer<T>(device, m_cpu, m_gpu,
            m_srv, m_uav);
    }
  public:
    std::vector<T> m_cpu;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_gpu;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_staging;
  public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;
};
} // namespace soku