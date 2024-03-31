#include "pch.h"
#include "Utils.h"

#include "GeometryGenerator.h"
#include "Mesh.h"

namespace soku {
	Mesh::Mesh() :
		indexCount(0)
	{
	}
	bool CubeMap::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::wstring& originalFilename,
		const std::wstring& diffuseFilename,
		const std::wstring& specularFilename)
	{
		auto data = GeometryGenerator::MakeCubeMapBox(15.f);
		cubeMesh = std::make_shared<Mesh>();
		std::vector<D3D11_INPUT_ELEMENT_DESC> elements{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

		Utils::CreateVertexBuffer(data.m_vertices, cubeMesh->vertexBuffer, device);
		Utils::CreateIndexBuffer(data.m_indices, cubeMesh->indexBuffer, device);
		cubeMesh->indexCount = data.m_indices.size();

		Utils::CreateVSAndInputLayout(elements, vertexShader, inputLayout, device, L"Cube");
		Utils::CreateConstantBuffer(vsConstantData, vsConstantBuffer, device);

		Utils::CreatePS(pixelShader, device, L"Cube");

		Utils::CreateCubeMapShaderResourceView(originalFilename, cubemapOrigin, device);
		if (diffuseFilename != L"") {
			Utils::CreateCubeMapShaderResourceView(diffuseFilename, cubemapDiffuse, device);
		}
		if (specularFilename != L"") {
			Utils::CreateCubeMapShaderResourceView(specularFilename, cubemapSpecular, device);
		}
		return true;
	}
	void CubeMap::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, cubeMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(cubeMesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(inputLayout.Get());

		context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());
		context->VSSetShader(vertexShader.Get(), 0, 0);

		context->PSSetShaderResources(0, 1, cubemapOrigin.GetAddressOf());
		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		context->PSSetShader(pixelShader.Get(), 0, 0);

		context->DrawIndexed(cubeMesh->indexCount, 0, 0);
	}
	void CubeMap::UpdateConstantData(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		Utils::UpdateConstantBuffer(vsConstantData, vsConstantBuffer, context);
		//Utils::UpdateConstantBuffer(psConstantData, psConstantBuffer, context);
	}
	bool MeshGroup::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::vector<MeshData<Vertex, uint32_t>>& meshData,
		const std::wstring& vsPrefix,
		const std::wstring& psPrefix)
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

		std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};

		for (const auto& data : meshData) {
			auto mesh = std::make_shared<Mesh>();
			Utils::CreateVertexBuffer(data.m_vertices, mesh->vertexBuffer, device);
			Utils::CreateIndexBuffer(data.m_indices, mesh->indexBuffer, device);
			mesh->indexCount = data.m_indices.size();
			Utils::CreateVSAndInputLayout(elements, vertexShader, inputLayout, device, vsPrefix);
			Utils::CreatePS(pixelShader, device, psPrefix);

			Utils::CreateVSAndInputLayout(elements, NormalVS, inputLayout, device, L"Normal");
			Utils::CreateGS(NormalGS, device, L"Normal");
			Utils::CreatePS(NormalPS, device, L"Normal");

			if (data.texturePath != "")
			{
				Utils::CreateShaderResourceView(data.texturePath, mesh->shaderResourceView, device);
			}
			meshes.push_back(mesh);
		}
		vsConstantData = { Matrix(),Matrix(), Matrix() };
		gsConstantData.length = 0.1f;
		Utils::CreateConstantBuffer(vsConstantData, vsConstantBuffer, device);
		Utils::CreateConstantBuffer(psConstantData, psConstantBuffer, device);
		Utils::CreateConstantBuffer(gsConstantData, gsConstantBuffer, device);

		return true;
	}
	void MeshGroup::UpdateConstantData(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		Utils::UpdateConstantBuffer(vsConstantData, vsConstantBuffer, context);
		Utils::UpdateConstantBuffer(psConstantData, psConstantBuffer, context);
	}
	void MeshGroup::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		ID3D11ShaderResourceView* iblShaders[] = {
			cubemapOrigin.Get(),
			cubemapDiffuse.Get(),
			cubemapSpecular.Get()
		};
		context->PSSetShaderResources(0, ARRAYSIZE(iblShaders), iblShaders);
		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		for (const auto m : meshes) {
			context->VSSetShader(vertexShader.Get(), 0, 0);
			context->PSSetShader(pixelShader.Get(), 0, 0);

			context->IASetVertexBuffers(0, 1, m->vertexBuffer.GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(m->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetInputLayout(inputLayout.Get());

			context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());
			context->PSSetConstantBuffers(0, 1, psConstantBuffer.GetAddressOf());

			context->PSSetShaderResources(ARRAYSIZE(iblShaders), 1, m->shaderResourceView.GetAddressOf());
			context->DrawIndexed(m->indexCount, 0, 0);

			if (drawNormal) {
				context->VSSetShader(NormalVS.Get(), 0, 0);
				ID3D11Buffer* constantBuffers[] = {
					vsConstantBuffer.Get(),
					gsConstantBuffer.Get()
				};
				context->GSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
				context->GSSetShader(NormalGS.Get(), 0, 0);
				context->PSSetShader(NormalPS.Get(), 0, 0);
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				context->DrawIndexed(m->indexCount, 0, 0);
				context->GSSetShader(nullptr, 0, 0);
			}

		}
	}
	void MeshGroup::UpdateModelWorld(const Matrix& modelWorldRow) {
		this->m_modelWorldRow = modelWorldRow;
		this->m_invTransposeRow = modelWorldRow;
		m_invTransposeRow.Translation(Vector3(0.0f));
		m_invTransposeRow = m_invTransposeRow.Invert().Transpose();

		vsConstantData.model = modelWorldRow.Transpose();
		vsConstantData.invTranspose = m_invTransposeRow.Transpose();
	}




}