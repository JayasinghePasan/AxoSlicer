struct PSInput
{
    float4 pos : SV_POSITION;
    float3 world : TEXCOORD0;
};

cbuffer GridSettings : register(b1)
{
    float spacing;
    float dpi;
    float2 origin;
};

float4 main(PSInput input) : SV_TARGET
{
    float2 rel = (input.world.xy - origin) / spacing;
    float2 grid = abs(frac(rel - 0.5f) - 0.5f) / (fwidth(rel) * dpi);

    float lineDist = min(grid.x, grid.y);
    float intensity = 1.0f - saturate(lineDist);

    // Axis lines for +X and +Y
    float axisX = (input.world.x > origin.x) ? abs(rel.y) / (fwidth(rel.y) * dpi * 2) : 10.0f;
    float axisY = (input.world.y > origin.y) ? abs(rel.x) / (fwidth(rel.x) * dpi * 2) : 10.0f;
    float axisXInt = 1.0f - saturate(axisX);
    float axisYInt = 1.0f - saturate(axisY);
    
    float3 color = float3(0.2f, 0.2f, 0.2f) * intensity;
    
    float alpha = intensity;
    if (axisXInt > axisYInt && axisXInt > intensity)
    {
        color = float3(1.0f, 0.0f, 0.0f);
        alpha = axisXInt;
    }
    else if (axisYInt > intensity)
    {
        color = float3(0.0f, 1.0f, 0.0f);
        alpha = axisYInt;
    }

    return float4(color, alpha);
}
