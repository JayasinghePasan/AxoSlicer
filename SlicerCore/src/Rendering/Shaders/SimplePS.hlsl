struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float diff = saturate(dot(normalize(input.normal), lightDir));
    float3 baseColor = float3(0.8f, 0.8f, 0.8f);
    float3 color = baseColor * (0.2f + 0.8f * diff);
    return float4(color, 1.0f);
}