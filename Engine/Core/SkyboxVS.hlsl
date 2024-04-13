#include "Header.hlsli"

SkyboxPSInput main(DefaultVSInput input)
{
    SkyboxPSInput output;
    output.WorldPos = input.position;

    float3 pos = mul(float4(input.position, 0.f), view).rgb;
    output.posProj = mul(float4(pos, 1.f), proj);
    return output;
}