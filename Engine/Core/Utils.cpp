#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Utils.h"
#include "stb_image_write.h"
#include <directxtk/DDSTextureLoader.h>

namespace soku {
using namespace Microsoft::WRL;
std::vector<uint8_t> Utils::CreateTextureImage(const std::wstring &filePath,
                                               int &width, int &height) {
    std::string path;
    int channels;
    path.assign(filePath.begin(), filePath.end());

    intptr_t handle;
    struct _finddata_t fd;
    if ((handle = _findfirst(path.c_str(), &fd)) == -1L) {
        std::cout << path << " file in directory!" << std::endl;
        return {};
    } else {
        unsigned char *img =
            stbi_load(path.c_str(), &width, &height, &channels, 0);
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
            for (size_t i = 0; i < width * height; i++) {
                for (size_t c = 0; c < 4; c++) {
                    image[4 * i + c] = img[i * channels + c];
                }
            }
        }
        std::cout << path << " " << width << " " << height << " " << std::endl;
        delete[] img;
        return image;
    }
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

void Utils::CreateDDSTexture(
    const std::wstring &filePath,
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &SRV,
    Microsoft::WRL::ComPtr<ID3D11Device> &device, bool isCubeMap) {

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    UINT miscFlags = 0;
    if (isCubeMap) {
        miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(
        device.Get(), filePath.c_str(), 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE, 0, miscFlags,
        DirectX::DDS_LOADER_FLAGS(false),
        (ID3D11Resource **)texture.GetAddressOf(), SRV.GetAddressOf(), NULL));
}

void Utils::CreateTexture(const std::wstring &filePath,
                          Microsoft::WRL::ComPtr<ID3D11Texture2D> &tex,
                          Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> &srv,
                          Microsoft::WRL::ComPtr<ID3D11Device> &device,
                          Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context,
                          bool useSRGB) {
    using Microsoft::WRL::ComPtr;
    DXGI_FORMAT format =
        useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

    ComPtr<ID3D11Texture2D> stagingTex;
    int width, height;
    const std::vector<uint8_t> image =
        CreateTextureImage(filePath, width, height);
    if (image.empty())
        return;
    CreateStagingTexture(width, height, image, format, stagingTex, device,
                         context);

    D3D11_TEXTURE2D_DESC texDesc;
    stagingTex->GetDesc(&texDesc);
    texDesc.MipLevels = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    device->CreateTexture2D(&texDesc, NULL, tex.GetAddressOf());
    context->CopySubresourceRegion(tex.Get(), 0, 0, 0, 0, stagingTex.Get(), 0,
                                   NULL);
    device->CreateShaderResourceView(tex.Get(), NULL, srv.GetAddressOf());
    context->GenerateMips(srv.Get());
}

void Utils::CreateStagingTexture(
    const int &width, const int &height, const std::vector<uint8_t> &image,
    const DXGI_FORMAT &format,
    Microsoft::WRL::ComPtr<ID3D11Texture2D> &stagingTex,
    Microsoft::WRL::ComPtr<ID3D11Device> &device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context, const int &mipLevels,
    const int &arraySize) {

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = format;
    texDesc.MipLevels = mipLevels;
    texDesc.ArraySize = arraySize;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(
        device->CreateTexture2D(&texDesc, NULL, stagingTex.GetAddressOf()));

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTex.Get(), 0, D3D11_MAP_WRITE, 0, &ms);
    uint8_t *pData = (uint8_t *)ms.pData;
    for (int h = 0; h < height; h++) {
        memcpy(&pData[h * ms.RowPitch], &image[h * 4 * width], 4 * width);
    }
    context->Unmap(stagingTex.Get(), 0);
}

void Utils::CreateTextureArray() {}
Matrix Utils::CreateReflectedMatrix(const Vector3 &n, const Vector3 &p) {
    float d = -n.Dot(p);
    Matrix mat(1.f - 2.f * n.x * n.x, -2.f * n.y * n.x, -2.f * n.z * n.x, 0.f,
               -2.f * n.x * n.y, 1.f - 2.f * n.y * n.y, -2.f * n.z * n.y, 0.f,
               -2.f * n.x * n.z, -2.f * n.y * n.z, 1.f - 2.f * n.z * n.z, 0.f,
               -2.f * n.x * d, -2.f * n.y * d, -2.f * n.z * d, 1.f);
    return mat;
}
} // namespace soku