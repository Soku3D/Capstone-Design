struct Particle
{
    float3 position;
    float3 color;
    float3 velocity;
    float width;
    float time;
};
#define PI 3.141592
RWStructuredBuffer<Particle> Particles : register(u0);
static const float delTime = 1.f / 144;

[numthreads(256,1,1)]
void main(uint dtID : SV_DispatchThreadID)
{
    Particle p = Particles[dtID];
    float w = 0.1f;
    float dTheta= (2.f * PI) * delTime * w;
    float2x2 rotMat = float2x2(cos(dTheta), sin(dTheta), -sin(dTheta), cos(dTheta));
    p.position.xy = mul(p.position.xy, rotMat);
 
    Particles[dtID] = p;
}