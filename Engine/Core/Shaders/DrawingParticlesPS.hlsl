#include "Header.hlsli"

struct PSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 texCoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};
float4 main(PSInput input) : SV_Target
{
    float r = length(input.texCoord - float2(0.5f, 0.5f));
    float3 color = exp(-30.f * r * r) * input.color;
    return float4(color, 1.f);
}
