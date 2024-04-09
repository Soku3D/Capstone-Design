#include "Header.hlsli"

SkyboxPSInput main(DefaultVSInput input)
{
    SkyboxPSInput output;
    output.WorldPos = input.position;

    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, viewProj);
    output.posProj = pos;

    return output;
}