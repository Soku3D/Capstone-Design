struct GSInput
{
    float4 pos : Position;
    float3 color : Color;
};
struct PSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(point GSInput input[1], uint primID : SV_PrimitiveID,
    inout TriangleStream<PSInput> outputStream)
{
    float4 x = float4(1, 0, 0, 0);
    float4 y = float4(0, 1, 0, 0);
    float w = 0.03f;
    
    PSInput output;
    output.primID = primID;
    output.color = input[0].color;
    
    output.pos = input[0].pos - (x + y) * w;
    output.uv = float2(0, 1);
    outputStream.Append(output);
    
    output.pos = input[0].pos - (x - y) * w;
    output.uv = float2(0, 0);
    outputStream.Append(output);
    
    output.pos = input[0].pos - (-x + y) * w;
    output.uv = float2(1, 1);
    outputStream.Append(output);
    
    output.pos = input[0].pos - (-x - y) * w;
    output.uv = float2(1, 0);
    outputStream.Append(output);
    
    
    
    outputStream.RestartStrip();
    
}