#include "Header.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler :register(s0);
cbuffer SamplingPSConstants :register(b0)
{
    float dx;
    float dy;
    float expose;
    float gamma;
};
float4 main(DefaultPSInput input) : SV_TARGET
{
    float3 invGamma = float3(1.f, 1.f, 1.f) / gamma;
	float3 color =  g_texture.Sample(g_sampler,input.texcoord).rgb;
    color = pow(color, invGamma);
    
    return float4(color, 1.f);
   
}