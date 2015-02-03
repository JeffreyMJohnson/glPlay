#version 330

in vec2 position;
in vec2 texCoord;

out vec2 UV;

uniform mat4 MVP;

void main()
{
	UV = texCoord;
	gl_Position = MVP * vec4(position, 0, 1);
}