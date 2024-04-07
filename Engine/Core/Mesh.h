#pragma once

namespace soku {
	struct Mesh {

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_albedoTexture;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_albedoSRV;
	
		UINT m_indexCount;
	};
}