cbuffer HighlightCB : register(b1)
{
    uint highlightMask;
    float3 _padding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input, uint primID : SV_PrimitiveID) : SV_TARGET
{
    float3 n = normalize(input.normal);
    float3 lightDir1 = normalize(float3( 0.5f,  1.0f, -0.5f));
    float3 lightDir2 = normalize(float3(-0.5f, -1.0f,  0.5f));
    
    float diff = saturate(dot(n, lightDir1)) + saturate(dot(n, lightDir2));
    diff *= 0.5;
    
    float3 baseColour = float3(0.7f, 0.7f, 0.7f);
    float3 finalColour = baseColour * (0.3f + 0.7f * diff);
    
    uint faceIndex = primID / 2;
    if ((highlightMask && (1u << faceIndex)) != 0)
        finalColour = float3(1.0f, 0.8f, 0.2f);
    
    return float4(finalColour, 1.0f);

}