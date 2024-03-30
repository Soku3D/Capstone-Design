#include "Header.hlsli"

TextureCube originCube : register(t0);
TextureCube diffuseCube : register(t1);
TextureCube specularCube : register(t2);
SamplerState g_sampler : register(s0);


float4 main(CubePSInput input) : SV_TARGET
{
	return originCube.Sample(g_sampler, input.modelPos);
}