#include "Header.hlsli"

RWTexture2D<float4> g_outputTex : register(u0);
Texture2D<float4> g_inputTex : register(t0);
SamplerState pointLinearSS : register(s0);

static const float weights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
static const int blurRadius = 5;

#define N 256
#define CACHE_SIZE (N+2*blurRadius)
groupshared float4 groupCache[CACHE_SIZE];

[numthreads(N, 1, 1)]
void main(uint3 gID : SV_GroupID, uint3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
    uint width;
    uint height;
    g_inputTex.GetDimensions(width, height);
    
    if (gtID.x < blurRadius)
    {
        uint x = max(dtID.x - blurRadius, 0);
        groupCache[gtID.x] = g_inputTex[int2(x, dtID.y)];
    }
    if (gtID.x >= N - blurRadius)
    {
        uint x = min(dtID.x + blurRadius, width - 1);
        groupCache[gtID.x + 2 * blurRadius] = g_inputTex[int2(x, dtID.y)];
    }
    groupCache[gtID.x + blurRadius] = g_inputTex[min(dtID.xy, uint2(width, height) - 1)];

    GroupMemoryBarrierWithGroupSync();
    
    float4 blurColor = float4(0.f, 0.f, 0.f, 0.f);
    //[unroll]
    for (int i = -blurRadius; i <= blurRadius; i++)
    {
        int idx = gtID.x + blurRadius + i;
        blurColor += weights[i + blurRadius] * groupCache[idx];
    }
    g_outputTex[dtID.xy] = blurColor;

}