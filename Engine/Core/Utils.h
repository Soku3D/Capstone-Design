#pragma once 


namespace soku {
	class Utils {
	public:
		static void CheckResult(HRESULT& hr, ID3DBlob* error)
		{
			if (FAILED(hr))
			{
				if (error)
				{
					std::cout << (char*)error->GetBufferPointer();
				}
			}
		}
		static void CreateVSAndInputLayout(
			std::vector<D3D11_INPUT_ELEMENT_DESC>& elements,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>& vs,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout,
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::wstring shaderfileName = L""
		);
		static void CreateGS(
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>& gs,
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::wstring shaderfileName = L""
		);
		static void CreatePS(
			Microsoft::WRL::ComPtr<ID3D11PixelShader>& ps,
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::wstring shaderfileName = L""
		);
		
		static std::vector<uint8_t> CreateTextureImage(const std::string& filePath, int& width, int& height);
		static void SavePNG(const std::vector<uint8_t>& image, const int& x, const int& y);
		template <typename VERTEX>
		static void CreateVertexBuffer(
			const std::vector<VERTEX>& vertices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
			Microsoft::WRL::ComPtr<ID3D11Device>& device)
		{
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = vertices.size() * sizeof(VERTEX);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.StructureByteStride = sizeof(VERTEX);

			D3D11_SUBRESOURCE_DATA subData = { 0, };
			subData.pSysMem = vertices.data();

			device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
		}
		template <typename INDEX>
		static void CreateIndexBuffer(
			const std::vector<INDEX>& indices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
			Microsoft::WRL::ComPtr<ID3D11Device>& device)
		{
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = indices.size() * sizeof(INDEX);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			bufferDesc.StructureByteStride = sizeof(INDEX);

			D3D11_SUBRESOURCE_DATA subData = { 0, };
			subData.pSysMem = indices.data();

			device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
			
		}
		template <typename CONSTANT>
		static void CreateConstantBuffer(
			const CONSTANT& constantData,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
			Microsoft::WRL::ComPtr<ID3D11Device>& device)
		{
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = sizeof(CONSTANT);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.StructureByteStride = sizeof(CONSTANT);

			D3D11_SUBRESOURCE_DATA subData = { 0, };
			subData.pSysMem = &constantData;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subData, buffer.GetAddressOf());
			if (FAILED(hr)) {
				std::cout << "CreateBuffer Failed\n";
			}
		}
		template <typename CONSTANT>
		static void UpdateConstantBuffer(const CONSTANT& constantData,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) 
		{
			D3D11_MAPPED_SUBRESOURCE ms;
			context->Map(constantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, &constantData, sizeof(CONSTANT));
			context->Unmap(constantBuffer.Get(), NULL);
		}
		
		static void UpdateTextureBuffer(const std::vector<uint8_t>& imageData,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
		{
			D3D11_MAPPED_SUBRESOURCE ms;
			context->Map(constantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
			memcpy(ms.pData, imageData.data(), sizeof(imageData));
			context->Unmap(constantBuffer.Get(), NULL);
		}
		static void CreateTextureBuffer(const std::string& filePath, 
			Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
			Microsoft::WRL::ComPtr<ID3D11Device>& device);
		static void CreateShaderResourceView(const std::string& filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView, Microsoft::WRL::ComPtr<ID3D11Device>& device);
		static void CreateCubeMapShaderResourceView(const std::wstring& filePath, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView, Microsoft::WRL::ComPtr<ID3D11Device>& device);
		static void CreateShaderResourceView(const std::string& filePath,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView,
			Microsoft::WRL::ComPtr<ID3D11Device>& device);
		static void CreateTextureArray(
			const std::vector<std::string>& filePaths,
			Microsoft::WRL::ComPtr<ID3D11Texture2D> textureArray,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView,
			Microsoft::WRL::ComPtr<ID3D11Device>& device);
	};
}