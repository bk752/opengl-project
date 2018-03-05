#ifndef DATA_H
#define DATA_H

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Color    // generic color class
{
	float r, g, b;  // red, green, blue
};

struct Vertex
{
	float x, y, z;
	size_t normal;
	Color color;
};

struct Triangle
{
	GLuint vertices[3];
};

#endif
