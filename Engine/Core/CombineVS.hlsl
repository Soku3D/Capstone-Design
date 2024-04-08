#include "Header.hlsli"

struct VSInput
{
    float3 position : POSITIONT;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};
DefaultPSInput main(VSInput input)
{
    DefaultPSInput output;

    output.posProj = float4(input.position, 1.f);
    
    output.texcoord = input.uv;

    return output;
}