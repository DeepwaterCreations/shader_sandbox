#version 330 core

out vec4 FragColor;
in vec3 vertColor;
in vec2 TexCoord;

uniform sampler2D inTexture;

void main()
{
	FragColor = texture(inTexture, TexCoord);
}

