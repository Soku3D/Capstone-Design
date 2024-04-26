#include "Header.hlsli"
struct GSInput
{
    float4 pos : Position;
    float3 color : Color;
};
struct Particle
{
    float3 pos;
    float3 color;
};
StructuredBuffer<Particle> particles : register(t20);

GSInput main(uint idx : SV_VertexID)
{
    Particle p = particles[idx];
    GSInput output;
    output.pos = float4(p.pos, 1.0);
    output.color = p.color;
    return output;
}
