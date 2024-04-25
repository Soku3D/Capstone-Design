
cbuffer GlobalConsts : register(b1)
{
    matrix viewProj;
    matrix view;
    matrix proj;
    float3 eyePos;
    float lod;
    
    int useEnv;
    float time;
}
TextureCube envIBL : register(t10);
TextureCube IrradianceIBL : register(t11);
TextureCube SpecularIBL : register(t12);
Texture2D brdf : register(t13);

struct DefaultVSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};
struct DefaultPSInput
{
    float4 posProj : SV_Position;
    float3 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};
struct SkyboxPSInput
{
    float4 posProj : SV_Position;
    float3 WorldPos : Position;
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