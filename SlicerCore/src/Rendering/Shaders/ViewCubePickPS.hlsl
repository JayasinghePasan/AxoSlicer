struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    float3 n = normalize(input.normal);
    float3 lightDir1 = normalize(float3(0.5f, 1.0f, -0.5f));
    float3 lightDir2 = normalize(float3(-0.5f, -1.0f, 0.5f));
    float diff = saturate(dot(n, lightDir1)) + saturate(dot(n, lightDir2));
    diff *= 0.5f;
    float3 baseColor = float3(1.0f, 1.0f, 0.0f);
    float3 color = baseColor * (0.3f + 0.7f * diff);
    return float4(color, 1.0f);
}