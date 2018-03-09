#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 normalTransform;
uniform mat4 prevmodelviewproj;
uniform mat4 modelviewproj;
//uniform vec3 pointPos;
//uniform vec3 spotPos;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec4 normalColor;
out vec3 FragPos;
out vec3 Normal;
out vec3 velocity;
out vec4 velocityColor;
out vec4 curPos;
out vec4 prevPos;

void main()
{
	curPos = modelviewproj * vec4(position, 1.0);
	prevPos = prevmodelviewproj * vec4(position, 1.0);
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position, 1.0);
    normalColor = vec4((normalize(normal) + vec3(1, 1, 1)) / 2, 1.0f);
    FragPos = (model * vec4(position, 1.0)).xyz;
    Normal = normalize((normalTransform * vec4(normal, 1.0)).xyz);
    //Normal = normal;
}
