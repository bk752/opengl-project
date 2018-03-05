#ifndef _WINDOW_H_
#define _WINDOW_H_

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

struct DirLight {
  glm::vec3 direction;
  glm::vec3 color;
};

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  float linear;
};

struct SpotLight {
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 color;
  float dropoff;
  float minDot;
};

class Window
{
public:
	static int width;
	static int height;
	static int mode;
	static bool activeLights[3];
	static bool normalColor;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static DirLight dir;
	static PointLight point;
	static SpotLight spot;
	static glm::vec3 viewPos;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow *, double, double);
	static void scroll_callback(GLFWwindow *, double, double);
};

#endif
