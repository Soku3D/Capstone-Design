#include "Header.hlsli"

Texture2D g_texture : register(t1);
SamplerState g_sampler :register(s0);

float4 main(DefaultPSInput input) : SV_TARGET
{
	return g_texture.Sample(g_sampler,input.texcoord);
}