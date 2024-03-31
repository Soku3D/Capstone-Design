struct BasicVSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : Normal;
};
struct BasicPSInput
{
    float4 posProj : SV_Position;
    float2 texcoord : Texcoord;
};
struct CubeVSInput
{
    float3 pos : POSITION;
};
struct CubePSInput
{
    float4 position : SV_Position;
    float3 modelPos : Position;
};
struct Light
{
    float3 lightPos;
    float fallOffStart;
    float3 lightStrength;
    float fallOffEnd;
    float3 lightDir;
    float spotPower;
};
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1;
    float3 specular;
    float dummy2;
    float3 fresnelR0;
    float dummy3;
};