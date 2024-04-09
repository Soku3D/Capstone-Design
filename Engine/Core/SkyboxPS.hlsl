#include "Header.hlsli"

SamplerState g_sampler : register(s0);
TextureCube envIBL : register(t10);

float4 main(SkyboxPSInput input) : SV_TARGET
{
    return envIBL.Sample(g_sampler, input.WorldPos);
}