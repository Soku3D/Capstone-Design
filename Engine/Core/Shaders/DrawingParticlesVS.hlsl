struct Particle
{
    float3 position;
    float3 color;
    float3 velocity;
    float3 acceleration;
    float width;
    float time;
    float mass;
    float density;
    float pressure;
};

struct GSInput
{
    float4 pos : SV_Position;
    float3 color : Color;
    float life : PSIZE0;
    float size : PSIZE1;
};
StructuredBuffer<Particle> particles : register(t0);

GSInput main(uint vertexID : SV_VertexID)
{
    GSInput output;
    Particle p = particles[vertexID];
    output.pos = float4(p.position, 1.f);
    float v = length(p.velocity);
    float cv = saturate(v / 5.f);;
    output.color = p.color + float3(cv, cv, 0.f);
    //output.color = float3(cv, cv,0.2f);
    output.size = p.width;
    output.life = p.time;
    return output;
}