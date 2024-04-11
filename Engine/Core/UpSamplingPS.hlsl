#include "Header.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingConstants : register(b0)
{
    float dx;
    float dy;
    float expose;
    float gamma;
}
float distribution[5] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f };

float4 main(DefaultPSInput input) : SV_TARGET
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    float3 a = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;

    float3 d = g_texture0.Sample(g_sampler, float2(x - dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + dx, y)).rgb;

    float3 g = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;
  
    return float4(color, 1.0);
}