#version 330

in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D TextureSample;

void main()
{
	outColor = texture(TextureSample, TexCoord) * vec4(1,1,1,1);
	//outColor = vec4(1,1,1,1);
}