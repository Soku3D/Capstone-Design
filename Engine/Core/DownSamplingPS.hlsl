#include "Header.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingConstants : register(b0)
{
    float dx;
    float dy;
    float expose;
    float gamma;
}
float distribution[5] = { 0.0545f, 0.2442f, 0.4026f ,0.2442f, 0.0545f };

float4 main(DefaultPSInput input) : SV_TARGET
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    float3 a = g_texture.Sample(g_sampler, float2(x - 2 * dx, y + 2 * dy)).rgb;
    float3 b = g_texture.Sample(g_sampler, float2(x, y + 2 * dy)).rgb;
    float3 c = g_texture.Sample(g_sampler, float2(x + 2 * dx, y + 2 * dy)).rgb;

    float3 d = g_texture.Sample(g_sampler, float2(x - 2 * dx, y)).rgb;
    float3 e = g_texture.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture.Sample(g_sampler, float2(x + 2 * dx, y)).rgb;

    float3 g = g_texture.Sample(g_sampler, float2(x - 2 * dx, y - 2 * dy)).rgb;
    float3 h = g_texture.Sample(g_sampler, float2(x, y - 2 * dy)).rgb;
    float3 i = g_texture.Sample(g_sampler, float2(x + 2 * dx, y - 2 * dy)).rgb;

    float3 j = g_texture.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 k = g_texture.Sample(g_sampler, float2(x + dx, y + dy)).rgb;
    float3 l = g_texture.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 m = g_texture.Sample(g_sampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;
  
    return float4(color, 1.0);
}