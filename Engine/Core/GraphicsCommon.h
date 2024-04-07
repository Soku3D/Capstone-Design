#pragma once
#include "GraphicsPSO.h"

namespace soku {
	namespace Graphics {
		void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	
		extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
		extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;

		extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRS;

		extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS;

		extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;

		extern GraphicsPSO defaultSolidPSO;
	}
}