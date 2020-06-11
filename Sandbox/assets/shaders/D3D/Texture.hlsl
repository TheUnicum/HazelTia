// Flat Color Shader

#type vertex
cbuffer CBuf
{
	matrix transform;
}

struct VSOut
{
	float2 tex : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tex : TexCoord)
	{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	vso.tex = tex;
    return vso;
}

#type fragment
Texture2D tex;
 
SamplerState splr;

float4 main(float2 tc : TexCoord) : SV_Target
	{
		return tex.Sample(splr, tc * 2);
	}
