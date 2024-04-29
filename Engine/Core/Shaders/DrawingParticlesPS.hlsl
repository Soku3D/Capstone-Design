struct PSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};
float smoothstep(float x)
{
    x = clamp(x, 0, 1);
    x = 1 - x;
    return x * x * (3 - 2.f * x);
}
float4 main(PSInput input) : SV_Target
{
    float2 pos = (input.uv - 0.5f)*2.f;
    input.color*=smoothstep(length(pos));
    return float4(input.color, 1.f);
}