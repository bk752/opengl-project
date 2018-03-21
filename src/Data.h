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

struct Mesh {
	std::vector<glm::vec3> normals;
	std::vector<Vertex> vertices;
	std::vector<Triangle> faces;
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 diffuse;
	float shininess;
}; 
struct Mesh2 {
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> faces;
};
#endif
