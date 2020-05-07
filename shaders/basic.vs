#version 440

in vec3 bPos;

uniform mat4 mvp = mat4(1.0);

void main()
{
	gl_Position = mvp * vec4(bPos, 1.0);
}
