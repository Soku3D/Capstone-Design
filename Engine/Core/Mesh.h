#pragma once

namespace soku {
struct Mesh {

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_aoTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_heightTexture;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aoSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_heightSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalSRV;

    UINT m_indexCount;
};
} // namespace soku