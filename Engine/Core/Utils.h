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
    static void
    SRVBarrier(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        ID3D11ShaderResourceView *nullSRVs[6] = {
            0,
        };
        context->PSSetShaderResources(0, 6, nullSRVs);
    }
    static void
    IABufferClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        ID3D11Buffer *nullBuffers[1] = {
            0,
        };
        UINT offset = 0;
        UINT stride = 0;
        context->IASetVertexBuffers(0, 1, nullBuffers, &stride, &offset);
    }
    static void
    ComputeShaderBarrier(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context) {
        ID3D11ShaderResourceView *nullSRV[6] = {
            0,
        };
        context->CSSetShaderResources(0, 6, nullSRV);
        ID3D11UnorderedAccessView *nullUAV[6] = {
            0,
        };
        context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
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

        device->CreateBuffer(&bufferDesc, &subData,
                             buffer.ReleaseAndGetAddressOf());
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

        device->CreateBuffer(&bufferDesc, &subData,
                             buffer.ReleaseAndGetAddressOf());
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

        HRESULT hr = device->CreateBuffer(&bufferDesc, &subData,
                                          buffer.ReleaseAndGetAddressOf());
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
    static void
    CreateTexture(const std::wstring &filePath,
                  Microsoft::WRL::ComPtr<ID3D11Texture2D> &tex,
                  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
                  Microsoft::WRL::ComPtr<ID3D11Device> &device,
                  Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                  bool useSRGB);
    static void
    CreateStagingTexture(const int &width, const int &height,
                         const std::vector<uint8_t> &image,
                         const DXGI_FORMAT &format,
                         Microsoft::WRL::ComPtr<ID3D11Texture2D> &stagingTex,
                         Microsoft::WRL::ComPtr<ID3D11Device> &device,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                         const int &mipLevels = 1, const int &arraySize = 1);
    template <typename T>
    static void
    CreateStagingBuffer(const std::vector<T> &cpu,
                        Microsoft::WRL::ComPtr<ID3D11Buffer> &staging,
                        Microsoft::WRL::ComPtr<ID3D11Device> &device) {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(T) * cpu.size();
        bufferDesc.Usage = D3D11_USAGE_STAGING;
        bufferDesc.CPUAccessFlags =
            D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        bufferDesc.StructureByteStride = sizeof(T);
        ThrowIfFailed(
            device->CreateBuffer(&bufferDesc, NULL, staging.GetAddressOf()));
    }
    static void CreateTextureArray();
    static Matrix CreateReflectedMatrix(const Vector3 &normal,
                                        const Vector3 &point);
    template <typename T>
    static void CreateStructuredBuffer(
        Microsoft::WRL::ComPtr<ID3D11Device> &device, std::vector<T> &cpu,
        Microsoft::WRL::ComPtr<ID3D11Buffer> &gpu,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> &uav,
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &rtv) {

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(T) * cpu.size();
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags =
            D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(T);

        D3D11_SUBRESOURCE_DATA subData;
        ZeroMemory(&subData, sizeof(subData));
        subData.pSysMem = cpu.data();
        ThrowIfFailed(
            device->CreateBuffer(&bufferDesc, &subData, gpu.GetAddressOf()));

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        ZeroMemory(&uavDesc, sizeof(uavDesc));
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = cpu.size();
        ThrowIfFailed(device->CreateUnorderedAccessView(gpu.Get(), &uavDesc,
                                                        uav.GetAddressOf()));
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.BufferEx.NumElements = cpu.size();
        ThrowIfFailed(device->CreateShaderResourceView(gpu.Get(), &srvDesc,
                                                       srv.GetAddressOf()));
    }
    static void
    CreateUATexture(const int &width, const int &height,
                    const DXGI_FORMAT &format,
                    Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    Microsoft::WRL::ComPtr<ID3D11Texture2D> &tex,
                    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
                    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> &uav,
                    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> &rtv);
};
} // namespace soku