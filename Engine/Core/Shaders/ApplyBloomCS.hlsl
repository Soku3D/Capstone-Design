RWTexture2D<float4> g_output : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 gtID : SV_GroupThreadID, uint3 gID : SV_GroupID, 
    uint3 dtID : SV_DispatchThreadID, uint gIDX : SV_GroupIndex)
{
    float2 idx = dtID.xy;
    
    float4 a = g_output[idx - float2(2.f, 2.f)];
    float4 b = g_output[idx - float2(2.f, 0.f)];
    float4 c = g_output[idx - float2(2.f, -2.f)];
    
    float4 d = g_output[idx - float2(2.f, 0.f)];
    float4 e = g_output[idx - float2(2.f, 0.f)];
    float4 f = g_output[idx - float2(-2.f, 0.f)];
    
    float4 g = g_output[idx - float2(2.f, -2.f)];
    float4 h = g_output[idx - float2(0.f, 2.f)];
    float4 i = g_output[idx - float2(-2.f, -2.f)];
   
    float4 j = g_output[idx - float2(1.f, 1.f)];
    float4 k = g_output[idx - float2(-1.f, -1.f)];
    float4 l = g_output[idx - float2(1.f, -1.f)];
    float4 m = g_output[idx - float2(-1.f, -1.f)];
    
    float3 color = float3(0.f, 0.f, 0.f);
    color += (j + k + l + m + e).rgb * 0.125f;
    color += (a + c + g + i).rgb * 0.03125f;
    color += (b + d + f + h).rgb * 0.0625f;
    g_output[dtID.xy] = float4(color * 2.f, 1.f);

}