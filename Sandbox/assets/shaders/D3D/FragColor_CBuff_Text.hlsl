// Flat Color Shader

#type vertex
cbuffer CBuf
{
	matrix transform;
}

struct VSOut
{
	float3 color : fragColor;
	float2 tex : fragTexCoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : inPosition, float3 color : inColor, float2 tex : inTexCoord)
	{
	VSOut vso;	
	vso.pos = mul(float4(pos, 1.0f), transform);
	//vso.pos = float4(pos, 1.0f);

	vso.color = color;
	vso.tex = tex;
	return vso;
}

#type fragment
Texture2D tex;
 
SamplerState splr;

float4 main(float3 clr : fragColor, float2 tc : fragTexCoord) : SV_Target
	{
		return tex.Sample(splr, tc);
	}
