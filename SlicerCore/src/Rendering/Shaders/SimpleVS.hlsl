#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 pos : SV_POSITION;    
    float3 normal : NORMAL;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(modelViewProj, float4(input.pos, 1.0f));
    output.normal = input.normal;
    return output;
}