struct Particle
{
    float3 position;
    float3 color;
    float3 velocity;
    float3 acceleration;
    float width;
    float time;
    float mass;
};
#define PI 3.141592
static const float3 g = float3(0, -9.8f, 0.f);
cbuffer CSConsts : register(b0)
{
    float dt;
}
RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(256,1,1)]
void main(uint dtID : SV_DispatchThreadID)
{
    
    Particle p = Particles[dtID];
    if (p.time > 0.3)
    {
        p.velocity += g * dt;
        p.position += p.velocity * dt;
    }
    else
    {
        
    }
    p.time += dt;
    Particles[dtID] = p;
}