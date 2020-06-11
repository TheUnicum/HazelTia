// Flat Color Shader

#type vertex
cbuffer CBuf
{
    matrix transform;
}

float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), transform);
}

#type fragment
cbuffer CBuf
{
    float4 face_colors[6];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
    return face_colors[(tid % 6) / 1];
}
