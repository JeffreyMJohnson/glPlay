#version 330

in vec2 position;
in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 MVP;

void main()
{
	TexCoord = texCoord;
	gl_Position = MVP * vec4(position, 0, 1);
}