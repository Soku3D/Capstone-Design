#include "Header.hlsli"
struct PSInput
{
    float4 posProj : SV_Position;
    float3 color : color;
};
struct Particle
{
    float3 pos;
    float3 color;
};
StructuredBuffer<Particle> particles : register(t20);

PSInput main(uint idx : SV_VertexID)
{
    Particle p = particles[idx];
    PSInput output;
    output.posProj = float4(p.pos, 1.0);
    output.color = p.color;

    return output;
}
