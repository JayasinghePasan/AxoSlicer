#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 world : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput o;
    float4 world = float4(input.pos, 1.0f);
    o.world = world.xyz;
    o.pos = mul(modelViewProj, world);
    return o;
}