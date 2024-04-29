struct Particle
{
    float3 position;
    float3 color;
    float3 velocity;
    float width;
};

struct GSInput
{
    float4 pos : SV_Position;
    float3 color : Color;
};
StructuredBuffer<Particle> particles : register(t0);

GSInput main(uint vertexID : SV_VertexID)
{
    GSInput output;
    Particle p = particles[vertexID];
    output.pos = float4(p.position, 1.f);
    output.color = p.color;    
    return output;
}