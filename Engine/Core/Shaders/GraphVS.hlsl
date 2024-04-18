#include "Header.hlsli"


DefaultPSInput main(DefaultVSInput input)
{
    DefaultPSInput output;
    output.normal = input.normal;
    float4 pos = float4(input.position, 1.0f);
    
    
    output.posWorld = pos;
    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;
	
    return output;
}