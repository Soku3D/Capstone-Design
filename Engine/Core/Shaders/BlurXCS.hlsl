#include "Header.hlsli"

RWTexture2D<float4> g_outputTex : register(u0);
Texture2D<float4> g_inputTex : register(t0);
SamplerState pointLinearSS : register(s0);

static const float weights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
static const int range = 5;
[numthreads(32, 32, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
    float3 blurColor = float3(0.f, 0.f, 0.f);
    float dx = 1.0 / 1920;
    float dy = 1.0 / 1080;
    float2 uv = float2((dtID.x + 0.5) * dx, (dtID.y + 0.5)*dy);
    
    for (int i = -range; i <= range; i++)
    {
        float3 color = g_inputTex.SampleLevel(pointLinearSS, uv + float2(i * dx, 0.0), 0.0).rgb;
        blurColor += color * weights[i + range];
    }
    g_outputTex[dtID.xy] = float4(blurColor, 1.f);

}