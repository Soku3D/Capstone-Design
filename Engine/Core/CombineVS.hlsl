#include "Header.hlsli"

DefaultPSInput main(DefaultVSInput input){
    DefaultPSInput output;

    output.posProj = input.position;
    output.texcoord = input.texcoord;

    return output;
}