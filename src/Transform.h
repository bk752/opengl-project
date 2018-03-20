#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

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
#include <list>
#include "Node.h"

class Transform : public Node
{
	std::list<Node *> children;
public:
	Transform(glm::mat4);
	glm::mat4 M;
	void draw(glm::mat4 c, bool blur);
	void update();
	void addChild(Node *);
	void removeChild(Node *);
};

#endif

