struct GeometryShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 color : COLOR;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float w = 0.3;
    float3 up = float3(0, 1, 0);
    float3 right = float3(1, 0, 0);
    
    PixelShaderInput output;
    output.pos.w = 1;
    output.color = input[0].color;
    
    output.pos.xyz = input[0].pos.xyz - w * right - w * up;
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);

    output.pos.xyz = input[0].pos.xyz - w * right + w * up;
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos.xyz = input[0].pos.xyz + w * right - w * up;
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos.xyz = input[0].pos.xyz + w * right + w * up;
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;

    outputStream.Append(output);

    outputStream.RestartStrip(); 
}
