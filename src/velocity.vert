#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location=0) in vec3 modelPos;
layout (location = 1) in vec3 normal;
uniform mat4 normalTransform;

uniform mat4 uModelViewProjectionMat;
uniform mat4 uPrevModelViewProjectionMat;

smooth out vec4 vPosition;
smooth out vec4 vPrevPosition;

void main() {
	vPosition = uModelViewProjectionMat * vec4(modelPos, 1.0);
	vPrevPosition = uPrevModelViewProjectionMat * vec4(modelPos, 1.0);
	vec2 UV = vPosition.xy/vPosition.w;
    vec3 Normal = (normalTransform * vec4(normal, 1.0)).xyz;
	vec4 velocity = vPosition - vPrevPosition;
	if (dot(Normal, velocity.xyz) > 0) {
		gl_Position = vPosition;
	} else {
		gl_Position = vPrevPosition;
	}
}