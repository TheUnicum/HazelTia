// Flat Color Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout (std140) uniform Matrices
{
	mat4 transformation;
};

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = transformation * vec4(a_Position, 1.0f);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

in vec2 v_TexCoord;

layout (std140) uniform BufColors
{
	vec4 face_colors[6];
};

uniform sampler2D u_Textures;

void main()
{
	//color = face_colors[gl_PrimitiveID % 6];
	color = texture(u_Textures, v_TexCoord);

}