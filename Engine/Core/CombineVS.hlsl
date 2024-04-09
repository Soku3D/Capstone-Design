#include "Header.hlsli"

DefaultPSInput main(DefaultVSInput input)
{
    DefaultPSInput output;

    output.posProj = float4(input.position, 1.f);
    
    output.texcoord = input.texcoord;

    return output;
}