#pragma once
#include "Constants.h"

namespace soku {
	template <typename Vertex_, typename Index_>
	class MeshData {
	public:
		MeshData() {}
		MeshData(std::vector<Vertex_>& vertices,
			std::vector<Index_>& indices,
			const std::string& texturePath = "") :
			m_vertices(vertices),
			m_indices(indices),
			texturePath(texturePath)
		{
		}
		std::vector<Vertex_> m_vertices;
		std::vector<Index_>  m_indices;
		std::string texturePath;
	};
	class Mesh {
	public:
		friend class MeshGroup;
		friend class CubeMap;
		Mesh();
	private:
		Microsoft::WRL::ComPtr <ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr <ID3D11Buffer> indexBuffer;
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

		UINT indexCount;
	};
	class CubeMap{
	public:
		CubeMap() {}
		bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::wstring& originalFilename = L"Assets/Textures/cubemaps/courtyard.dds.dds",
			const std::wstring& diffuseFilename = L"", 
			const std::wstring& specularFilename = L"");
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
	public:
		void UpdateConstantData(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

		CubeVSConstantData vsConstantData;
		BasicPSConstantData psConstantData;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapOrigin;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapDiffuse;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapSpecular;
	private:
		std::shared_ptr<Mesh> cubeMesh;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> psConstantBuffer;

		Microsoft::WRL::ComPtr <ID3D11SamplerState> samplerState;

	};
	class MeshGroup {
	public:
		bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::vector<MeshData<Vertex,uint32_t>>& meshData,
			const std::wstring& vsPrefix = L"",
			const std::wstring& psPrefix = L"" );
		void UpdateConstantData(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
		void UpdateModelWorld(const Matrix& modelWorldRow);
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
	public: 
		Matrix m_modelWorldRow = Matrix();
		Matrix m_invTransposeRow = Matrix();
		BasicVSConstantData vsConstantData;
		BasicPSConstantData psConstantData;
		NormalGSConstantData gsConstantData;
		bool drawNormal = false;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapOrigin;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapDiffuse;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubemapSpecular;
	private:
		std::vector<std::shared_ptr<Mesh>> meshes;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> psConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> gsConstantBuffer;


		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> NormalVS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> NormalGS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> NormalPS;
		
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	};
	typedef MeshData<Vertex, uint32_t> meshData;
}
