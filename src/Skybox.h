#ifndef SKYBOX_H_
#define SKYBOX_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "OBJObject.h"
#include "shader.h"

#define VERTEX_SKY_PATH "sky.vert"
#define FRAGMENT_SKY_PATH "sky.frag"

class Skybox {
	GLuint VBO, VAO;
	GLint skyShader;
	unsigned int cubemapTexture;

public:
	Skybox(std::vector<std::string> faces);
	~Skybox();

	void draw();
	static unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif
