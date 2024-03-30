#include "Header.hlsli"


SamplerState g_sampler : register(s0);
Texture g_texture : register(t0);

float4 main(BasicPSInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}