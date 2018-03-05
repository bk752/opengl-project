#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Node.h"
#include "Data.h"

class Geometry : public Node
{
private:
  //std::vector<unsigned int> indices;
  glm::vec3 normal;
  glm::vec3 origin;
  float angle;

public:
	Geometry(std::string, glm::vec3, glm::vec3, glm::vec3, float, GLuint);
	~Geometry();

	void drawModel(GLuint);
	void draw(glm::mat4);
	void update() {};
	void parse(std::string);

	glm::vec3 albedo;
	glm::vec3 ambient;
	glm::vec3 specular;
	float phongExp;
	GLuint shader;

	glm::vec3 position;
	glm::vec3 scale;
	std::vector<Vertex> vertices;
	std::vector<glm::vec3> normals;
	std::vector<Triangle> faces;
	glm::mat4 toWorld;
	glm::mat4 normalTransform;
	glm::mat4 rotation;

	GLuint VBO, VB1, VAO, EBO;
};

#endif