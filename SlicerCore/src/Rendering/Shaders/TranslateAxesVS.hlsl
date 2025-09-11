#include "Common.hlsli"

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL; 
    // Passing the axis type using the normal
    // (1,0,0)=X, (0,1,0)=Y, (0,0,1)=Z
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 axisId : AXIS;
};

VSOutput main(VSInput input)
{
    VSOutput o;
    o.pos = mul(modelViewProj, float4(input.pos, 1.0f));
    o.axisId = input.normal;
    return o;
}