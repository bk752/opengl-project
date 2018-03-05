#ifndef _BEZIER_H_
#define _BEZIER_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Node.h"

class Bezier
{
	void addPoints();

	std::vector<glm::vec3> controls;
	std::vector<glm::vec3> points;
	GLint shader;
	GLuint VBO, VAO;
public:
	Bezier(glm::vec3, glm::vec3, glm::vec3, glm::vec3, GLint);
	std::pair<glm::vec3, glm::vec3> interpolate(float);
	int length();
	void draw();
	void updateBuffer();
	void addControl(glm::vec3, glm::vec3);
};

#endif

