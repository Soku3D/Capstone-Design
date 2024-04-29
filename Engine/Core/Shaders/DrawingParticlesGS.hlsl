struct GSInput
{
    float4 pos : SV_Position;
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
    float w = 0.03f;
    PSInput output;
    output.primID = primID;
    output.color = input[0].color;
    float4 pos = input[0].pos;
    
    output.uv = float2(0, 1);
    output.pos = pos + float4(-w, -w, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(0, 0);
    output.pos = pos + float4(-w, w, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(1, 1);
    output.pos = pos + float4(w, -w, 0, 0);
    outputStream.Append(output);
    
    output.uv = float2(1, 0);
    output.pos = pos + float4(w, w, 0, 0);
    outputStream.Append(output);
    
    outputStream.RestartStrip();
}