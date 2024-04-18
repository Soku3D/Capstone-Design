#include "Header.hlsli"

cbuffer MeshConsts : register(b0) {
	Matrix world;
	Matrix worldIT;
    float heightScale;
    float3 dummy;
}
Texture2D heightTex : register(t0);
SamplerState g_sampler : register(s0);

DefaultPSInput main(DefaultVSInput input) 
{
    DefaultPSInput output;
    float4 normal = float4(input.normal, 0.0f);
    output.normal = mul(normal, worldIT).xyz;
    output.normal = normalize(output.normal);
    
	float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, world);
    
    float height = heightTex.SampleLevel(g_sampler, input.texcoord,0.f).r;
    if (height > 0)
    {
        height = height * 2.f - 1.f;
        pos += float4(height * output.normal * heightScale, 0.f);
    }
    
    output.posWorld = pos;
	pos = mul(pos, viewProj);

	output.posProj = pos;
	output.texcoord = input.texcoord;
	
	return output;
}