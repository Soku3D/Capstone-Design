#include "Header.hlsli"

cbuffer BasicVSConstant : register(b0) {
	matrix model;
	matrix view;
	matrix projection;
	matrix invTranspose;
};

BasicPSInput main( BasicVSInput input)
{
	BasicPSInput output;
	output.texcoord = input.texcoord;

	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.posProj = pos;

	return output;
}