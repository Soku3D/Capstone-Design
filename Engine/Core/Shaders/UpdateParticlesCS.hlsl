#include "Header.hlsli"

struct Particle
{
    float3 pos;
    float3 color;
};
RWStructuredBuffer<Particle> particles : register(u0);
static const float dt = 1.f / 144.f;
[numthreads(256,1,1)]
void main(uint dtID: SV_DispatchThreadID, uint3 gID : SV_GroupID)
{
    Particle p = particles[dtID.x]; 
    
    float3 velocity = float3(-p.pos.y, p.pos.x, 0.0) * 0.5;

    p.pos += velocity * dt;
    
    particles[dtID.x].pos = p.pos;
}