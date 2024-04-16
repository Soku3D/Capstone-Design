#include "Header.hlsli"

Texture2D origin : register(t0);
Texture2D blur : register(t1);
SamplerState g_sampler :register(s0);

cbuffer SamplingPSConstants :register(b0)
{
    float dx;
    float dy;
    float expose;
    float gamma;
    float bloomStrength;
};
float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1.f, 1.f, 1.f) / gamma;
    color = clamp(color * expose, 0.f, 1.f);
    color = pow(color, invGamma);
    return color;
}
float4 main(DefaultPSInput input) : SV_TARGET
{
   
    float3 color0 = origin.Sample(g_sampler, input.texcoord).rgb;
    float3 color1 = blur.Sample(g_sampler, input.texcoord).rgb;
    float3 color = color0 * (1.f - bloomStrength) + color1 * bloomStrength;
    color = LinearToneMapping(color);
    
    return float4(color, 1.f);
}