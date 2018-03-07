#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;

out vec3 vPos;

void main()
{
	vPos = (model * vec4(position, 1.0f)).xyz;
}
