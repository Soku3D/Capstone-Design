#pragma once
#include "GraphicsPSO.h"

namespace soku {
namespace Graphics {
void InitCommonStates(Microsoft::WRL::ComPtr<ID3D11Device> &device);

extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearWrapSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> linearClampSS;
extern Microsoft::WRL::ComPtr<ID3D11SamplerState> pointClampSS;

extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> solidRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyboxRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> reflectedRS;
extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> reflectedSkyboxRS;

extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> drawDSS;
extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskingDSS;
extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskedDrawDSS;

extern Microsoft::WRL::ComPtr<ID3D11InputLayout> dummyIL;
extern Microsoft::WRL::ComPtr<ID3D11InputLayout> basicIL;
extern Microsoft::WRL::ComPtr<ID3D11InputLayout> combineIL;

extern Microsoft::WRL::ComPtr<ID3D11BlendState> basicBS;
extern Microsoft::WRL::ComPtr<ID3D11BlendState> addBS;

extern GraphicsPSO defaultSolidPSO;
extern GraphicsPSO reflectedSolidPSO;
extern GraphicsPSO skyboxPSO;
extern GraphicsPSO reflectedSkyboxPSO;
extern GraphicsPSO combinePSO;
extern GraphicsPSO upSamplingPSO;
extern GraphicsPSO downSamplingPSO;
extern GraphicsPSO stencliMaskPSO;
extern GraphicsPSO mirrorPSO;
extern GraphicsPSO blendPSO;
extern GraphicsPSO graphPSO;
extern GraphicsPSO billboardPSO;
extern GraphicsPSO drawingParticlesPSO;
extern GraphicsPSO drawingParticlesBlendPSO;

extern ComputePSO InitPSO;
extern ComputePSO bloomPSO;
extern ComputePSO blurXPSO;
extern ComputePSO blurYPSO;
extern ComputePSO blurXGroupCachePSO;
extern ComputePSO blurYGroupCachePSO;
extern ComputePSO updateParticlePSO;
extern ComputePSO disspateParticlePSO;
} // namespace Graphics
} // namespace soku