#include "Header.hlsli"

cbuffer CubeVSConstant : register(b0)
{
    matrix view;
    matrix projection;
}
CubePSInput main(CubeVSInput input)
{
    CubePSInput output;
    output.modelPos = input.pos;

    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.position = pos;

    return output;
}