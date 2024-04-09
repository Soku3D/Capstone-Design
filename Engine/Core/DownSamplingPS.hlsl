#include "Header.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingConstants : register(b0)
{
    float dx;
    float dy;
    float2 dummy1;
}
float distribution[5] = { 0.1f, 0.2f, 0.4f ,0.2f, 0.1f };

float4 main(DefaultPSInput input) : SV_TARGET
{
    float3 color = g_texture.Sample(g_sampler, input.texcoord).rgb;
    //for (int i = -2; i <=2; i++)
    //{
    //    input.texcoord.x+dx*i;
    //}
    return  float4(color, 1.f);
}