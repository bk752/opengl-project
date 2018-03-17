#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 UV;
void main()
{
	UV = vec2((aPos+ 1) / 2);
    gl_Position = vec4(aPos, 1.0);
}  