RWTexture2D<float4> outputTex : register(u0);

[numthreads(1,1,1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    float3 color = outputTex[dtID.xy].rgb;
    color = max(0, color-0.1f);
    outputTex[dtID.xy] = float4(color, 1.f);

}