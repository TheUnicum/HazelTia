// Flat Color Shader

#type vertex

struct VSOut
{
    float3 col : aColor;
    float4 pos : SV_Position;
};


VSOut main(float3 pos : Position, float3 col : Color)
{
    VSOut vso;
    vso.pos = float4(pos, 1.0f);
    vso.col = col;
    return vso;
}

#type fragment
float4 main(float3 col : aColor) : SV_Target
{
    return float4(col, 1.0f);
}
