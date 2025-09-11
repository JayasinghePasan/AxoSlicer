struct PSInput
{
    float4 pos : SV_POSITION;
    float3 axisId : AXIS;
};

float4 main(PSInput i) : SV_TARGET
{
    // Hard-coded axis colors
    if (i.axisId.x > 0.5)
        return float4(1, 0, 0, 1); // X
    if (i.axisId.y > 0.5)
        return float4(0, 1, 0, 1); // Y
    if (i.axisId.z > 0.5)
        return float4(0, 0, 1, 1); // Z
    return float4(1, 1, 1, 1);
}
