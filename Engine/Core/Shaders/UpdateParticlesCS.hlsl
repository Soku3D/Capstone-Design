#include "Header.hlsli"

struct Particle
{
    float3 pos;
    float3 color;
};
RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(256,1,1)]
void main(uint dtID: SV_DispatchThreadID, uint3 gID : SV_GroupID)
{
    particles[dtID.x] = particles[dtID.x];
}