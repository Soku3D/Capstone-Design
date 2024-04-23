Texture2D g_input : register(t0);
RWTexture2D<float4> g_output : register(u0);
SamplerState g_sampler : register(s0);

static const float weights[11] =
{
    0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
};

static const int blurRadius = 5;

[numthreads(32, 32, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
       
    float dx = 1.0 / 1920;
    float dy = 1.0 / 1080;
    
    float2 uv = float2((dtID.x + 0.5) * dx, (dtID.y + 0.5) * dy);
    float3 blurColor = float3(0, 0, 0);

    [unroll]
    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        float3 color = g_input.SampleLevel(g_sampler, uv + float2(float(i) * dx, 0.0),0.0).rgb;
        blurColor += weights[i + blurRadius] * color;
    }
    g_output[dtID.xy] = float4(blurColor, 1);
}
