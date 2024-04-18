#pragma once

namespace soku {
inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw std::exception();
    }
}
class Utils {
  public:
    static void CheckResult(HRESULT &hr, ID3DBlob *error) {
        if (FAILED(hr)) {
            if (error) {
                std::cout << (char *)error->GetBufferPointer();
            }
        }
    }
    static void ResetSRV(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        std::vector<ID3D11ShaderResourceView *> pSRV{nullptr};
        context->PSSetShaderResources(0, pSRV.size(), pSRV.data());
    }
    static std::vector<uint8_t> CreateTextureImage(const std::wstring &filePath,
                                                   int &width, int &height);
    static void SavePNG(const std::vector<uint8_t> &image, const int &x,
                        const int &y);
    template <typename VERTEX>
    static void
    CreateVertexBuffer(const std::vector<VERTEX> &vertices,
                       Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer,
                       Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(VERTEX));
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.StructureByteStride = sizeof(VERTEX);

        D3D11_SUBRESOURCE_DATA subData = {
            0,
        };
        subData.pSysMem = vertices.data();

        device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
    }
    template <typename INDEX>
    static void
    CreateIndexBuffer(const std::vector<INDEX> &indices,
                      Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer,
                      Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(INDEX));
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
        bufferDesc.StructureByteStride = sizeof(INDEX);

        D3D11_SUBRESOURCE_DATA subData = {
            0,
        };
        subData.pSysMem = indices.data();

        device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
    }
    template <typename CONSTANT>
    static void
    CreateConstantBuffer(const CONSTANT &constantData,
                         Microsoft::WRL::ComPtr<ID3D11Buffer> &buffer,
                         Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(CONSTANT);
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.StructureByteStride = sizeof(CONSTANT);

        D3D11_SUBRESOURCE_DATA subData = {
            0,
        };
        subData.pSysMem = &constantData;

        HRESULT hr =
            device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "Create ConstantBuffer  Failed\n";
        }
    }
    template <typename CONSTANT>
    static void
    UpdateConstantBuffer(const CONSTANT &constantData,
                         Microsoft::WRL::ComPtr<ID3D11Buffer> &constantBuffer,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(constantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL,
                     &ms);
        memcpy(ms.pData, &constantData, sizeof(CONSTANT));
        context->Unmap(constantBuffer.Get(), NULL);
    }
    static void
    UpdateTextureBuffer(const std::vector<uint8_t> &imageData,
                        Microsoft::WRL::ComPtr<ID3D11Buffer> &constantBuffer,
                        Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(constantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL,
                     &ms);
        memcpy(ms.pData, imageData.data(), sizeof(imageData));
        context->Unmap(constantBuffer.Get(), NULL);
    }
    static void CreateDDSTexture(
        const std::wstring &filePath,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &shaderResourceView,
        Microsoft::WRL::ComPtr<ID3D11Device> &device, bool isCubeMap);
    static void CreateTexture(
        const std::wstring &filePath, Microsoft::WRL::ComPtr<ID3D11Texture2D> &tex,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
        Microsoft::WRL::ComPtr<ID3D11Device> &device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context, bool useSRGB);
    static void
    CreateStagingTexture(const int &width, const int &height,
                         const std::vector<uint8_t> &image,
                         const DXGI_FORMAT &format,
                         Microsoft::WRL::ComPtr<ID3D11Texture2D> &stagingTex,
                         Microsoft::WRL::ComPtr<ID3D11Device> &device,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                         const int &mipLevels = 1, const int &arraySize = 1);
    static void CreateTextureArray();
    static Matrix CreateReflectedMatrix(const Vector3 &normal,
                                        const Vector3 &point);
};
} // namespace soku