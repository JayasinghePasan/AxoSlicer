struct PSInput
{
    float4 pos : SV_POSITION;
    float3 world : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float spacing = 5.0f; // world-space grid spacing
    float2 coord = input.world.xy / spacing;

    float2 grid = abs(frac(coord - 0.5f) - 0.5f) / fwidth(coord);

    float lineDist = min(grid.x, grid.y);
    float intensity = 1.0f - saturate(lineDist);

    float3 color = float3(0.2f, 0.2f, 0.2f) * intensity;
    return float4(color, intensity);
}