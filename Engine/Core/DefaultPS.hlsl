#include "Header.hlsli"

Texture2D albedoTex : register(t0);
Texture2D aoTex : register(t1);
Texture2D normalTex : register(t2);
SamplerState g_sampler : register(s0);

float4 main(DefaultPSInput input) : SV_TARGET
{
    return albedoTex.SampleLevel(g_sampler, input.texcoord, lod);
}