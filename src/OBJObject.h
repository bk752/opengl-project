#ifndef OBJOBJECT_H
#define OBJOBJECT_H

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
#include "Data.h"

class OBJObject
{
private:
	//std::vector<unsigned int> indices;
	glm::vec3 normal;
	glm::vec3 origin;
	float angle;

public:

	glm::mat4 prevmodelviewproj = glm::mat4(1.0);
	OBJObject(std::string, glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3, float);
	~OBJObject();
	void assimpLoad(std::string);
	void parse(std::string);
	void draw(GLuint);

	void move(float, float, float, float);
	void reset(bool);

	void update();
	void spin(float);
	void rotate(glm::vec3, float);

	glm::vec3 albedo;
	glm::vec3 ambient;
	glm::vec3 specular;
	float phongExp;
	bool refresh = true;
	glm::vec3 position;
	glm::vec3 scale;
	std::vector<Vertex> vertices;
	std::vector<glm::vec3> normals;
	std::vector<Triangle> faces;
	glm::mat4 toWorld;
	glm::mat4 normalTransform;
	glm::mat4 rotation;
	std::vector<Mesh> meshes;
	GLuint VBO, VB1, VAO, EBO;
	std::vector<GLuint> vaos;
};

#endif
