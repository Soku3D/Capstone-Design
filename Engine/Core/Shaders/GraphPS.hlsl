#include "Header.hlsli"
#define PI 3.141592
float4 main(DefaultPSInput input) : SV_TARGET
{
    float pi_2 = PI * 2.0;
    float2 center = float2(0.5f, 0.5f);
    float range = 10.f;
    float x = ((input.texcoord.x - 0.5f) * range*10.f) / 0.5f;
    float y = (-input.texcoord.y + 0.5f) * range / 0.5f;
    float alpha = 1.f;
    float lamda = 1.f;
    float k = pi_2 / lamda;
    float t = 1.f;
    float w = pi_2 / t;
    float fx = cos((k * x - w * time));
    float sinxx = sin(x) / x;
    float gauusian = exp(-alpha*(x * x) / time);
   
    if (abs(y - sinxx) < 0.01f || abs(x) < 0.01f || abs(y) < 0.01f)
        return float4(1.f, 0.f, 0.f, 1.f);
    else
        return float4(1.f, 1.f, 1.f, 1.f);
}