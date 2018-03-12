#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
smooth in vec4 vPosition;
smooth in vec4 vPrevPosition;

out vec2 oVelocity;

void main(void) {
    vec2 a = (vPosition.xy / vPosition.w) * 0.5;
    vec2 b = (vPrevPosition.xy / vPrevPosition.w) * 0.5;
    oVelocity = a - b;
}