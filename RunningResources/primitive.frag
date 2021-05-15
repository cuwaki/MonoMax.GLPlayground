#version 330 core

uniform vec3 vertexColorForFragment;
out vec4 color;

void main()
{
	color = vec4(vertexColorForFragment, 1.);
}
