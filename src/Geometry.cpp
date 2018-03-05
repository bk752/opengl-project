#include "Geometry.h"
#include "Window.h"
#include <cmath>

Geometry::Geometry(std::string name, glm::vec3 diff, glm::vec3 amb, glm::vec3 spec, float phong, GLuint program) 
{
	toWorld = glm::mat4(1.0f);
	parse(name);

	this->albedo = diff;
	this->ambient = amb;
	this->specular = spec;
	this->phongExp = phong;
	this->shader = program;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VB1);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Triangle), &faces[0], GL_STATIC_DRAW);

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
	float r, g, b;
	int c1, c2;

	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	FILE *fp = fopen(name.c_str(), "rb");

	if (fp == NULL) {
		std::cerr << "error loading file: `" << name << "`" << std::endl;
		exit(-1);
	}

	size_t v = 0;
	size_t vn = 0;
	while (((c1 = fgetc(fp)) != EOF) && ((c2 = fgetc(fp)) != EOF)) {
		if ((c1 == 'v') && (c2 == ' ')) {
			fscanf(fp, "%f %f %f %f %f %f\n", &x, &y, &z, &r, &g, &b);

			maxX = std::max(maxX, x);
			maxY = std::max(maxY, y);
			maxZ = std::max(maxZ, z);

			minX = std::min(minX, x);
			minY = std::min(minY, y);
			minZ = std::min(minZ, z);

			if (v < normals.size()) {
				float nx = normals[v][0];
				float ny = normals[v][1];
				float nz = normals[v][2];
				float length = sqrt(nx * nx + ny * ny + nz * nz);
				vertices.push_back({x, y, z, v, {
					(1 + nx / length) / 2,
					(1 + ny / length) / 2,
					(1 + nz / length) / 2
				}});
			} else {
				vertices.push_back({x, y, z, 0, {r, g, b}});
			}
			v++;
		} else if ((c1 == 'v') && (c2 == 'n')) {
			fscanf(fp, "%f %f %f\n", &x, &y, &z);

			normals.push_back(glm::vec3(x, y, z));
if (vn < vertices.size()) {
				vertices[vn].normal = vn;
				float length = sqrt(x * x + y * y + z * z);
				vertices[vn].color.r = (1 + x / length) / 2;
				vertices[vn].color.g = (1 + y / length) / 2;
				vertices[vn].color.b = (1 + z / length) / 2;
			}
			vn++;
		} else if ((c1 == 'f') && (c2 == ' ')) {
			size_t v1 = 0;
			size_t v2 = 0;
			size_t v3 = 0;
			size_t n1 = 0;
			size_t n2 = 0;
			size_t n3 = 0;

			fscanf(fp, "%u//%u %u//%u %u//%u\n", &v1, &n1, &v2, &n2, &v3, &n3);

			faces.push_back({{((unsigned int)v1)-1, ((unsigned int)v2)-1, ((unsigned int)v3)-1}});
		} else {
			fscanf(fp, "%*[^\n]\n", NULL);
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
	drawModel(shader);
} 

void Geometry::drawModel(GLuint shaderProgram) 
{
	this->normalTransform = glm::mat4(glm::mat3(this->toWorld));

	//std::cout<<normalTransform[0][0]<<" "<<normalTransform[0][1]<<" "<<normalTransform[0][2]<<" "<<normalTransform[0][3]<<"  ";
	//std::cout<<normalTransform[1][0]<<" "<<normalTransform[1][1]<<" "<<normalTransform[1][2]<<" "<<normalTransform[1][3]<<"  ";
	//std::cout<<normalTransform[2][0]<<" "<<normalTransform[2][1]<<" "<<normalTransform[2][2]<<" "<<normalTransform[2][3]<<"  ";
	//std::cout<<normalTransform[3][0]<<" "<<normalTransform[3][1]<<" "<<normalTransform[3][2]<<" "<<normalTransform[3][3]<<"\n";
	this->normalTransform = glm::inverse(glm::transpose(this->normalTransform));

	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	glBegin(GL_TRIANGLES);
	// Loop through all the vertices of this OBJ Object and render them
	for (Triangle t : faces) {
		Vertex v = vertices[t.vertices[0]];
		glVertex3f(v.x, v.y, v.z);

		v = vertices[t.vertices[1]];
		glVertex3f(v.x, v.y, v.z);

		v = vertices[t.vertices[2]];
		glVertex3f(v.x, v.y, v.z);
	}

	glm::mat4 modelview = Window::V * toWorld;

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelview"), 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "normalTransform"), 1, GL_FALSE, &normalTransform[0][0]);

	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &Window::viewPos[0]);

	glUniform1i(glGetUniformLocation(shaderProgram, "useNormal"), Window::normalColor);

	glUniform1i(glGetUniformLocation(shaderProgram, "showDirLight"), Window::activeLights[0]);
	glUniform1i(glGetUniformLocation(shaderProgram, "showPointLight"), Window::activeLights[1]);
	glUniform1i(glGetUniformLocation(shaderProgram, "showSpotLight"), Window::activeLights[2]);

	glUniform3fv(glGetUniformLocation(shaderProgram, "Albedo"), 1, &this->albedo[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "AmbientReflect"), 1, &this->ambient[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "SpecularReflect"), 1, &this->specular[0]);
	glUniform1f(glGetUniformLocation(shaderProgram, "PhongExp"), this->phongExp);

	glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.direction"), 1, &Window::dir.direction[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.color"), 1, &Window::dir.color[0]);

	glUniform3fv(glGetUniformLocation(shaderProgram, "pointLight.position"), 1, &Window::point.position[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "pointLight.color"), 1, &Window::point.color[0]);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLight.linear"), Window::point.linear);

	glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.position"), 1, &Window::spot.position[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.direction"), 1, &Window::spot.direction[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram, "spotLight.color"), 1, &Window::spot.color[0]);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.dropoff"), Window::spot.dropoff);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.minDot"), cos(Window::spot.minDot));

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 3*faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnd();

	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}