RWTexture2D<float4> g_output : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 gtID : SV_GroupThreadID, uint3 gID : SV_GroupID, 
    uint3 dtID : SV_DispatchThreadID, uint gIDX : SV_GroupIndex)
{
    float4 white = float4(1.f, 1.f, 1.f, 1.f);
    float4 grey = float4(0.4f, 0.4f, 0.4f, 1.f);
    int gtIDx = gtID.x;
    int gtIDy = gtID.y;
    
    if (gtIDx == gtIDy || gtIDx + gtIDy == 31)
    {
        g_output[dtID.xy] = white;
    }
    else
    {
        g_output[dtID.xy] = grey;
    }
}