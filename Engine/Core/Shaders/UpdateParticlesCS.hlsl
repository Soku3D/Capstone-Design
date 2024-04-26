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
    float2 dir = float2(-particles[dtID].pos.y, particles[dtID].pos.x);
    float velocity = 1.f;
    particles[dtID.x].pos.xy += dir*velocity * dt;
}