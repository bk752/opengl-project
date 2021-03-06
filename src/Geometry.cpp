#include "Geometry.h"
#include "Window.h"
#include <cmath>
#include <algorithm>

Geometry::Geometry(std::string name, glm::vec3 diff, glm::vec3 amb, glm::vec3 spec, float phong, GLuint program, GLuint shadow, bool adjacent) 
{
	toWorld = glm::mat4(1.0f);

	storeAdjacent = adjacent;
	parse(name);

	this->albedo = diff;
	this->ambient = amb;
	this->specular = spec;
	this->phongExp = phong;


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VB1);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		(GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VB1);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		(GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint), &faces[0], GL_STATIC_DRAW);

	this->shader = program;

	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "modelview");
	uModel = glGetUniformLocation(shader, "model");
	uNormalTransform = glGetUniformLocation(shader, "normalTransform");
	uViewPos = glGetUniformLocation(shader, "viewPos");
	uUseNormal = glGetUniformLocation(shader, "useNormal");
	uShowDir = glGetUniformLocation(shader, "showDirLight");
	uShowPoint = glGetUniformLocation(shader, "showPointLight");
	uShowSpot = glGetUniformLocation(shader, "showSpotLight");
	uAlbedo = glGetUniformLocation(shader, "Albedo");
	uAmbRef = glGetUniformLocation(shader, "AmbientReflect");
	uSpecRef = glGetUniformLocation(shader, "SpecularReflect");
	uPhongExp = glGetUniformLocation(shader, "PhongExp");
	uDirLightDir = glGetUniformLocation(shader, "dirLight.direction");
	uDirLightColor = glGetUniformLocation(shader, "dirLight.color");
	uPointLightPos = glGetUniformLocation(shader, "pointLight.position");
	uPointLightColor = glGetUniformLocation(shader, "pointLight.color");
	uPointLightAtten = glGetUniformLocation(shader, "pointLight.linear");
	uSpotLightPos = glGetUniformLocation(shader, "spotLight.position");
	uSpotLightDir = glGetUniformLocation(shader, "spotLight.direction");
	uSpotLightColor = glGetUniformLocation(shader, "spotLight.color");
	uSpotLightDrop = glGetUniformLocation(shader, "spotLight.dropoff");
	uSpotLightMinDot = glGetUniformLocation(shader, "spotLight.minDot");
	uPrevMvp = glGetUniformLocation(shader, "uPrevModelViewProjectionMat");
	uCurrentMvp = glGetUniformLocation(shader, "uModelViewProjectionMat");

	this->shadowShader = shadow;
	uShadowModel = glGetUniformLocation(shadowShader, "model");
	uShadowViewProject = glGetUniformLocation(shadowShader, "viewproject");
	uShadowLightPos = glGetUniformLocation(shadowShader, "lightPos");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Geometry::~Geometry()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Geometry::parse(std::string name) 
{
	float x, y, z;
	float r = -1;
	float g = -1;
	float b = -1;
	int c1, c2;

	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	FILE *fp = fopen(name.c_str(), "rb");

	if (fp == NULL) {
		std::cerr << "error loading file: `" << name << "`" << std::endl;
		exit(-1);
	}

	std::map<std::pair<GLuint, GLuint>, GLuint> edgeOpposite;
	std::vector<GLuint> plainFaces;
	while (((c1 = fgetc(fp)) != EOF) && ((c2 = fgetc(fp)) != EOF)) {
		if ((c1 == 'v') && (c2 == ' ')) {
			fscanf(fp, "%f %f %f %f %f %f\n", &x, &y, &z, &r, &g, &b);

			maxX = std::max(maxX, x);
			maxY = std::max(maxY, y);
			maxZ = std::max(maxZ, z);

			minX = std::min(minX, x);
			minY = std::min(minY, y);
			minZ = std::min(minZ, z);

			vertices.push_back(glm::vec3(x, y, z));
		} else if ((c1 == 'v') && (c2 == 'n')) {
			fscanf(fp, "%f %f %f\n", &x, &y, &z);

			normals.push_back(glm::vec3(x, y, z));
		} else if ((c1 == 'f') && (c2 == ' ')) {
			GLuint v[3] = {0, 0, 0};
			GLuint n[3] = {0, 0, 0};

			fscanf(fp, "%u//%u %u//%u %u//%u\n", v, n, v + 1, n + 1, v + 2, n + 2);

			if (storeAdjacent) {
				for (int i = 0; i < 3; i++) {
					plainFaces.push_back(v[i]);
					edgeOpposite[std::pair<GLuint, GLuint>(
									v[i], v[(i + 1) % 3])] = v[(i + 2) % 3];
				}
			} else {
				faces.push_back(v[0]-1);
				faces.push_back(v[1]-1);
				faces.push_back(v[2]-1);
			}
		} else {
			fscanf(fp, "%*[^\n]\n", NULL);
		}
	}
	for (int face = 0; face < plainFaces.size() / 3; face++) { 
		GLuint *v = &plainFaces[3 * face];
		for (int i = 0; i < 3; i++) {
			faces.push_back(v[i] - 1);
			auto opposite = edgeOpposite.find(std::pair<GLuint, GLuint>(
							v[(i + 1) % 3], v[i]));
			if (opposite == edgeOpposite.end()) {
				faces.push_back(v[(i + 2) % 3] - 1);
				std::cout<<"No opposite\n";
			} else {
				faces.push_back((opposite->second) - 1);
			}
		}
	}
	std::cout<<faces.size()<<'\n';
	fclose(fp);

	float medX = (maxX + minX) / 2;
	float medY = (maxY + minY) / 2;
	float medZ = (maxZ + minZ) / 2;

	float radX = (maxX - minX) / 2;
	float radY = (maxY - minY) / 2;
	float radZ = (maxZ - minZ) / 2;
	float rad = std::max(std::max(radX, radY), radZ);
	std::cout<<minX<<" "<<maxX<<" "<<minY<<" "<<maxY<<" "<<minZ<<" "<<maxZ<<"\n";
	std::cout<<medX<<" "<<medY<<" "<<medZ<<"\n";
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].x -= medX;
		vertices[i].y -= medY;
		vertices[i].z -= medZ;

		vertices[i].x /= rad;
		vertices[i].y /= rad;
		vertices[i].z /= rad;
	}
}

void Geometry::draw(glm::mat4 c) {
	this->toWorld = c;
	GLint program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if ((GLuint)program == shader) {
		drawModel();
	} else if ((GLuint)program == shadowShader) {
		drawShadow();
	}
} 

void Geometry::drawModel() 
{
	this->normalTransform = glm::mat4(glm::mat3(this->toWorld));

	//std::cout<<normalTransform[0][0]<<" "<<normalTransform[0][1]<<" "<<normalTransform[0][2]<<" "<<normalTransform[0][3]<<"  ";
	//std::cout<<normalTransform[1][0]<<" "<<normalTransform[1][1]<<" "<<normalTransform[1][2]<<" "<<normalTransform[1][3]<<"  ";
	//std::cout<<normalTransform[2][0]<<" "<<normalTransform[2][1]<<" "<<normalTransform[2][2]<<" "<<normalTransform[2][3]<<"  ";
	//std::cout<<normalTransform[3][0]<<" "<<normalTransform[3][1]<<" "<<normalTransform[3][2]<<" "<<normalTransform[3][3]<<"\n";
	this->normalTransform = glm::inverse(glm::transpose(this->normalTransform));

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glm::mat4 modelview = Window::V * toWorld;
	glm::mat4 modelviewproj = Window::P * Window::V * toWorld;
	prevmodelviewproj = modelviewproj;


	glUniformMatrix4fv(uPrevMvp, 1, GL_FALSE, &prevmodelviewproj[0][0]);
	glUniformMatrix4fv(uCurrentMvp, 1, GL_FALSE, &modelviewproj[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uNormalTransform, 1, GL_FALSE, &normalTransform[0][0]);

	glUniform3fv(uViewPos, 1, &Window::viewPos[0]);

	glUniform1i(uUseNormal, Window::normalColor);

	glUniform1i(uShowDir, Window::activeLights[0]);
	glUniform1i(uShowPoint, Window::activeLights[1]);
	glUniform1i(uShowSpot, Window::activeLights[2]);

	glUniform3fv(uAlbedo, 1, &this->albedo[0]);
	glUniform3fv(uAmbRef, 1, &this->ambient[0]);
	glUniform3fv(uSpecRef, 1, &this->specular[0]);
	glUniform1f(uPhongExp, this->phongExp);

	glUniform3fv(uDirLightDir, 1, &Window::dir.direction[0]);
	glUniform3fv(uDirLightColor, 1, &Window::dir.color[0]);

	glUniform3fv(uPointLightPos, 1, &Window::point.position[0]);
	glUniform3fv(uPointLightColor, 1, &Window::point.color[0]);
	glUniform1f(uPointLightAtten, Window::point.linear);

	glUniform3fv(uSpotLightPos, 1, &Window::spot.position[0]);
	glUniform3fv(uSpotLightDir, 1, &Window::spot.direction[0]);
	glUniform3fv(uSpotLightColor, 1, &Window::spot.color[0]);
	glUniform1f(uSpotLightDrop, Window::spot.dropoff);
	glUniform1f(uSpotLightMinDot, cos(Window::spot.minDot));

	glBindVertexArray(VAO);

	if (storeAdjacent) {
		glDrawElements(GL_TRIANGLES_ADJACENCY, faces.size(), GL_UNSIGNED_INT, 0);
	} else {
		glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

void Geometry::update() {
}

void Geometry::drawShadow() {
	if (storeAdjacent) {
		glm::mat4 viewproject = Window::P * Window::V;
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
		glUniformMatrix4fv(uShadowViewProject, 1, GL_FALSE, &viewproject[0][0]);
		glUniformMatrix4fv(uShadowModel, 1, GL_FALSE, &toWorld[0][0]);
		glUniform3fv(uShadowLightPos, 1, &Window::point.position[0]);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES_ADJACENCY, faces.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}
