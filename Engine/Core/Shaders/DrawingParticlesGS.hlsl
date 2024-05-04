struct GSInput
{
    float4 pos : SV_Position;
    float3 color : Color;
    float life : PSIZE0;
    float size : PSIZE1;
};
struct PSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};
cbuffer Constants : register(b0)
{
    float radio;
}

[maxvertexcount(4)]
void main(point GSInput input[1], uint primID : SV_PrimitiveID,
    inout TriangleStream<PSInput> outputStream)
{
    float h = input[0].size;
    float w = h * radio;
    PSInput output;
    output.primID = primID;
    output.color = input[0].color;
    float4 pos = input[0].pos;
    
    output.uv = float2(0, 1);
    output.pos = pos + float4(-w, -h, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(0, 0);
    output.pos = pos + float4(-w, h, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(1, 1);
    output.pos = pos + float4(w, -h, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(1, 0);
    output.pos = pos + float4(w, h, 0, 0);
    outputStream.Append(output);
    
    outputStream.RestartStrip();
}