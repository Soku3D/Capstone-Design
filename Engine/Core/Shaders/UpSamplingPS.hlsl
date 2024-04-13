#include "Header.hlsli"

Texture2D g_texture : register(t0);
SamplerState LinearClampSS : register(s0);

//   1      | 1 2 1 |
//  ---  *  | 2 4 2 |
//  16      | 1 2 1 | 

// 0.0625 0.125 0.25
cbuffer SamplingConstants : register(b0)
{
    float dx;
    float dy;
    float expose;
    float gamma;
}

float4 main(DefaultPSInput input) : SV_TARGET
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    float3 a = g_texture.Sample(LinearClampSS, float2(x - 1.f * dx, y - 1.f * dy)).rgb;
    float3 b = g_texture.Sample(LinearClampSS, float2(x, y - 2.f * dy)).rgb;
    float3 c = g_texture.Sample(LinearClampSS, float2(x + 1.f * dx, y - 1.f * dy)).rgb;
    
    float3 d = g_texture.Sample(LinearClampSS, float2(x - 1.f * dx, y)).rgb;
    float3 e = g_texture.Sample(LinearClampSS, float2(x, y)).rgb;
    float3 f = g_texture.Sample(LinearClampSS, float2(x + 1.f * dx, y)).rgb;
    
    float3 g = g_texture.Sample(LinearClampSS, float2(x - 1.f * dx, y + 1.f * dy)).rgb;
    float3 h = g_texture.Sample(LinearClampSS, float2(x, y + 1.f * dy)).rgb;
    float3 i = g_texture.Sample(LinearClampSS, float2(x + 1.f * dx, y + 1.f * dy)).rgb;
    
    float3 color = float3(0.f, 0.f, 0.f);
    color += (a + c + g + i) * 0.0625f;
    color += (b + d + f + h) * 0.125f;
    color += e * 0.25f;
    
    return float4(color, 1.f);
}