#include "Header.hlsli"

float4 main(DefaultPSInput input) : SV_TARGET
{
    float2 center = float2(0.5f, 0.5f);
    
    float x = ((input.texcoord.x - 0.5f) * 3.141592f) / 0.5f;
    float y = (-input.texcoord.y + 0.5f) * 3.141592f / 0.5f;
    
    float gauusian = exp(-(x * x) / 2.f);
   
    if (abs(y - gauusian) < 0.01f || abs(x) < 0.01f || abs(y) < 0.01f)
        return float4(1.f, 0.f, 0.f, 1.f);
    else
        return float4(1.f, 1.f, 1.f, 1.f);
}