#include "Header.hlsli"

Texture2D g_texture2d : register(t0);
SamplerState g_sampler : register(s0);

cbuffer SamplingConstants : register(b0)
{
    float dx;
    float dy;
    float2 dummy;
}
float distribution[5] = { 0.1f, 0.2f, 0.4f ,0.2f, 0.1f };

float4 main(DefaultPSInput input) : SV_TARGET
{
    for (int i = -2; i <=2; i++)
    {
        input.texcoord.x+dx*i;
    }

}