#include "Header.hlsli"

cbuffer MeshConsts : register(b0) {
	Matrix world;
	Matrix worldIT;
}
DefaultPSInput main(DefaultVSInput input) 
{
    DefaultPSInput output;
	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, world);
	pos = mul(pos, viewProj);

	output.posProj = pos;
	output.texcoord = input.texcoord;
	
	return output;
}