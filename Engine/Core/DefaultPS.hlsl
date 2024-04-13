#include "Header.hlsli"

Texture2D albedoTex : register(t0);
Texture2D aoTex : register(t1);
Texture2D normalTex : register(t2);
SamplerState g_sampler : register(s0);

cbuffer MaterialConstants : register(b0)
{
    int useAlbedo;
    int useNormal;
    int useAO;
    int useRoughness;
};

float4 main(DefaultPSInput input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.f);
    if (useAlbedo)
    {
        color = albedoTex.SampleLevel(g_sampler, input.texcoord, lod);
    }
    else
    {
        color = float4(1.f, 1.f, 0.f, 1.f);
    }
    return color;
    //return albedoTex.SampleLevel(g_sampler, input.texcoord, lod);
}