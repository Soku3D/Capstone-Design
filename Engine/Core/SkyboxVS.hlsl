#include "Header.hlsli"

SkyboxPSInput main(DefaultVSInput input)
{
    SkyboxPSInput output;
    output.WorldPos = input.position;

    float4 pos = float4(input.position, 0.0f);
    pos = mul(pos, view);
    pos.w = 1.f;
    pos = mul(pos, proj);
    output.posProj = pos;

    return output;
}