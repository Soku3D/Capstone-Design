#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Utils.h"
#include "stb_image_write.h"
#include <directxtk/DDSTextureLoader.h>

#include "CompiledShaders\CubePs.h"
#include "CompiledShaders\CubeVS.h"

namespace soku {
using namespace Microsoft::WRL;
std::vector<uint8_t> Utils::CreateTextureImage(const std::string &filePath,
                                               int &width, int &height) {
    int channels;
    unsigned char *img =
        stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    std::vector<uint8_t> image(width * height * 4);
    if (channels == 3) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int idx = j + width * i;
                image[idx * 4] = img[idx * 3];
                image[idx * 4 + 1] = img[idx * 3 + 1];
                image[idx * 4 + 2] = img[idx * 3 + 2];
                image[idx * 4 + 3] = 255;
            }
        }
    } else {
        memcpy(image.data(), img, image.size());
    }
    delete[] img;
    return image;
}
void Utils::SavePNG(const std::vector<uint8_t> &image, const int &x,
                    const int &y) {
    time_t temp;
    struct tm *timeinfo;

    time(&temp);
    timeinfo = localtime(&temp);

    std::string currtime = "Results/";
    currtime += std::to_string(timeinfo->tm_year + 1900);
    currtime += "_";
    currtime += std::to_string(timeinfo->tm_mon + 1);
    currtime += "_";
    currtime += std::to_string(timeinfo->tm_mday);
    currtime += "_";
    currtime += std::to_string(timeinfo->tm_hour);
    currtime += "_";
    currtime += std::to_string(timeinfo->tm_min);
    currtime += "_";
    currtime += std::to_string(timeinfo->tm_sec);
    currtime += ".png";

    stbi_write_png(currtime.c_str(), x, y, 4, image.data(), x * 4);
}
void Utils::CreateTextureBuffer(
    const std::string &filePath,
    Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    int width, height;
    std::vector<uint8_t> image = CreateTextureImage(filePath, width, height);

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = image.data();
    subData.SysMemPitch = width * 4;

    HRESULT hr = device->CreateTexture2D(&textureDesc, &subData,
                                         texture.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        std::cout << "CreateTexture2D FAILED\n";
    }
}
void Utils::CreateShaderResourceView(
    const std::string &filePath,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &shaderResourceView,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    CreateTextureBuffer(filePath, texture, device);
    HRESULT hr = device->CreateShaderResourceView(
        texture.Get(), nullptr, shaderResourceView.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        std::cout << "CreateShaderResourceView FAILED\n";
    }
}
void Utils::CreateCubeMapShaderResourceView(
    const std::wstring &filePath,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &shaderResourceView,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    //
    HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
        device.Get(), filePath.c_str(), 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE,
        DirectX::DX11::DDS_LOADER_FLAGS(false),
        (ID3D11Resource **)texture.GetAddressOf(),
        shaderResourceView.GetAddressOf(), nullptr);
    if (FAILED(hr)) {
        std::cout << "CreateShaderResourceView FAILED\n";
    }
}
void Utils::CreateShaderResourceView(
    const std::string &filePath,
    Microsoft::WRL::ComPtr<ID3D11Texture2D> &texture,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &shaderResourceView,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    CreateTextureBuffer(filePath, texture, device);
    HRESULT hr = device->CreateShaderResourceView(
        texture.Get(), nullptr, shaderResourceView.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        std::cout << "CreateShaderResourceView FAILED\n";
    }
}
void Utils::CreateTextureArray(
    const std::vector<std::string> &filePaths,
    Microsoft::WRL::ComPtr<ID3D11Texture2D> textureArray,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &shaderResourceView,
    Microsoft::WRL::ComPtr<ID3D11Device> &device) {
    int width = 0, height = 0, channels = 4;
    std::vector<uint8_t> pixels;
    for (const auto &path : filePaths) {
        std::vector<uint8_t> img =
            Utils::CreateTextureImage(path, width, height);
        pixels.insert(pixels.end(), img.begin(), img.end());
    }
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = (UINT)width;
    textureDesc.Height = (UINT)height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = (UINT)filePaths.size();
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> subDataArray(textureDesc.ArraySize);
    for (int i = 0; i < subDataArray.size(); i++) {
        int offset = width * height * 4 * i * sizeof(uint8_t);
        D3D11_SUBRESOURCE_DATA subData;
        ZeroMemory(&subData, sizeof(subData));
        subData.pSysMem = pixels.data() + offset;
        subData.SysMemPitch = width * 4;
        subData.SysMemSlicePitch = width * height * 4;
        subDataArray[i] = subData;
    }

    HRESULT hr = device->CreateTexture2D(&textureDesc, subDataArray.data(),
                                         textureArray.GetAddressOf());
    if (FAILED(hr)) {
        std::cout << "CreateTexture2DArray FAILED\n";
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    ZeroMemory(&viewDesc, sizeof(viewDesc));
    viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    viewDesc.ViewDimension =
        D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    viewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
    viewDesc.Texture2DArray.MipLevels = textureDesc.MipLevels;
    viewDesc.Texture2DArray.MostDetailedMip = 0;
    viewDesc.Texture2DArray.FirstArraySlice = 0;

    hr = device->CreateShaderResourceView(textureArray.Get(), &viewDesc,
                                          shaderResourceView.GetAddressOf());

    if (FAILED(hr)) {
        std::cout << "CreateShaderResourceView Array FAILED\n";
    }
}
} // namespace soku