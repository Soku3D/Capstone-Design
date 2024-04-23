RWTexture2D<float4> g_output : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 gtID : SV_GroupThreadID, uint3 gID : SV_GroupID, 
    uint3 dtID : SV_DispatchThreadID, uint gIDX : SV_GroupIndex)
{
    float4 red = float4(50000.0f, 0.f, 0.f, 1.f);
    float4 green = float4(0.f, 50000.0f, 0.f, 1.f);
    float4 blue = float4(0.f, 0.f, 50000.0f, 1.f);
    float4 black = float4(0.f, 0.f, 0.f, 1.f);
    if (dtID.x == 480 && dtID.y == 540)
    {
        g_output[dtID.xy] = red;
    }
    else if (dtID.x == 960 && dtID.y == 540)
    {
        g_output[dtID.xy] = green;
    }
    else if (dtID.x == 1440 && dtID.y == 540)
    {
        g_output[dtID.xy] = blue;
    }
    else
    {
        g_output[dtID.xy] = black;
        
    }

}