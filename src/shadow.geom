#version 330

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

in vec3 vPos[];

uniform vec3 lightPos;
uniform mat4 viewproject;

float EPSILON = 0.01;

// Emit a quad made by projecting an edge to infinity and connecting the ends
void projectEdge(vec3 v0, vec3 v1)
{    
    vec3 lightDir = normalize(v0 - lightPos);   
    gl_Position = viewproject * vec4((v0 + lightDir * EPSILON), 1.0);
    EmitVertex();
 
    gl_Position = viewproject * vec4(lightDir, 0.0);
    EmitVertex();
    
    lightDir = normalize(v1 - lightPos);
    gl_Position = viewproject * vec4((v1 + lightDir * EPSILON), 1.0);
    EmitVertex();
    
    gl_Position = viewproject * vec4(lightDir , 0.0);
    EmitVertex();

    EndPrimitive();            
}


void main()
{
    vec3 e1 = vPos[2] - vPos[0];
    vec3 e2 = vPos[4] - vPos[0];
    vec3 e3 = vPos[1] - vPos[0];
    vec3 e4 = vPos[3] - vPos[2];
    vec3 e5 = vPos[4] - vPos[2];
    vec3 e6 = vPos[5] - vPos[0];

    vec3 normal = normalize(cross(e1,e2));
    vec3 lightDir = normalize(lightPos - vPos[0]);

    if (dot(normal, lightDir) > 0) {

        normal = cross(e3,e1);

        if (dot(normal, lightDir) <= 0) {
            projectEdge(vPos[0], vPos[2]);
        }

        normal = cross(e4,e5);
        lightDir = lightPos - vPos[2];

        if (dot(normal, lightDir) <= 0) {
            projectEdge(vPos[2], vPos[4]);
        }

        normal = cross(e2,e6);
        lightDir = lightPos - vPos[4];

        if (dot(normal, lightDir) <= 0) {
            projectEdge(vPos[4], vPos[0]);
        }

        lightDir = (normalize(vPos[0] - lightPos));
        gl_Position = viewproject * vec4((vPos[0] + lightDir * EPSILON), 1.0);
        EmitVertex();

        lightDir = (normalize(vPos[2] - lightPos));
        gl_Position = viewproject * vec4((vPos[2] + lightDir * EPSILON), 1.0);
        EmitVertex();

        lightDir = (normalize(vPos[4] - lightPos));
        gl_Position = viewproject * vec4((vPos[4] + lightDir * EPSILON), 1.0);
        EmitVertex();
        EndPrimitive();
 
        // render the back cap
        lightDir = vPos[0] - lightPos;
        gl_Position = viewproject * vec4(lightDir, 0.0);
        EmitVertex();

        lightDir = vPos[4] - lightPos;
        gl_Position = viewproject * vec4(lightDir, 0.0);
        EmitVertex();

        lightDir = vPos[2] - lightPos;
        gl_Position = viewproject * vec4(lightDir, 0.0);
        EmitVertex();

        EndPrimitive();
    } else {
        gl_Position = vec4(2, 0, 0, 0.0);
        EmitVertex();
        gl_Position = vec4(2, 0, 0, 0.0);
        EmitVertex();
        gl_Position = vec4(2, 0, 0, 0.0);
        EmitVertex();
        EndPrimitive();
	}
}

