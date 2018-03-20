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
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "Node.h"
#include "Data.h"

class Geometry : public Node
{
private:
  glm::vec3 normal;
  glm::vec3 origin;
  float angle;

public:
	Geometry(std::string, glm::vec3, glm::vec3, glm::vec3, float, GLuint, GLuint, GLuint, bool);
	~Geometry();

	void drawVelocity();
	void update();
	void drawModel(bool blur);
	void drawShadow();
	void draw(glm::mat4, bool blur);
	void parse(std::string);

	float storeAdjacent;

	glm::vec3 albedo;
	glm::vec3 ambient;
	glm::vec3 specular;
	float phongExp;
	GLuint shader;
	GLuint shadowShader;
	GLuint ambianceShader;

	glm::mat4 prevView;
	glm::mat4 prevProj;

	GLuint velocityShader;
	glm::mat4 prevmodelviewproj;
	glm::vec3 position;
	glm::vec3 scale;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> faces;
	glm::mat4 toWorld;
	glm::mat4 normalTransform;
	glm::mat4 rotation;
	std::vector<Mesh> meshes;
	std::vector<GLuint> vaos;

	GLuint uVelocityNormalTransform, uVelocityMVP, uVelocityPrevMVP;
	GLuint VBO, VB1, VAO, EBO;
	GLuint uProjection, uModelview, uModel, uNormalTransform;
	GLuint uViewPos, uAlbedo, uAmbRef, uSpecRef, uPhongExp, uUseNormal;
	GLuint uDirLightDir, uDirLightColor;
	GLuint uPointLightPos, uPointLightColor, uPointLightAtten;
	GLuint uSpotLightPos, uSpotLightDir, uSpotLightColor, uSpotLightDrop, uSpotLightMinDot;
	GLuint uShowSpot, uShowDir, uShowPoint;
	GLuint uPrevMvp, uCurrentMvp;
	GLuint uBlur;

	GLuint uShadowModel, uShadowViewProject, uShadowLightPos;
};

#endif
