#version 440

out vec3 fragColor;

uniform vec3 color = vec3(1.0);

void main()
{
	fragColor = color;
}
