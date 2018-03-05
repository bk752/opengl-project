#include <cmath>
#include <iostream>
#include "Bezier.h"
#include "Window.h"

Bezier::Bezier(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4, GLint shade) {
  controls.push_back(c1);
  controls.push_back(c2);
  controls.push_back(c3);
  controls.push_back(c4);
  addPoints();

  shader = shade;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

int Bezier::length() {
  return controls.size() / 4;
}

std::pair<glm::vec3, glm::vec3> Bezier::interpolate(float d) {
  int segment = ((int)d) % length();
  float t = fmod(d, 1.0f);
  float t2 = t * t;
  float t3 = t2 * t;
  float b0 = -(t3) + 3 * t2 - 3 * t + 1;
  float b1 = 3 * t3 - 6 * t2 + 3 * t;
  float b2 = -3 * ( t3) + 3 * t2;
  float b3 = t3;

  float db0 = -3 * (t2) + 6 * t - 3;
  float db1 = 9 * t2 - 12 * t + 3;
  float db2 = -9 * (t2) + 6 * t;
  float db3 = 3 * t2;
  return std::pair<glm::vec3, glm::vec3>(
      b0 * controls[4 * segment]
    + b1 * controls[4 * segment + 1]
    + b2 * controls[4 * segment + 2]
    + b3 * controls[4 * segment + 3],
      db0 * controls[4 * segment]
    + db1 * controls[4 * segment + 1]
    + db2 * controls[4 * segment + 2]
    + db3 * controls[4 * segment + 3]
  );
}
void Bezier::addControl(glm::vec3 control, glm::vec3 endPoint) {
  controls.push_back(controls.back());
  controls.push_back(controls.back() + controls.back() - controls[controls.size() - 3]);
  controls.push_back(control);
  controls.push_back(endPoint);
  addPoints();
}

void Bezier::addPoints() {
  for (float i = length() - 1; i < length(); i += 1 / 150.0f) {
    glm::vec3 val = interpolate(i).first;
    points.push_back(val);
  }
}

void Bezier::updateBuffer() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3*sizeof(float),
		(GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Bezier::draw() {
  glUseProgram(shader);

	GLuint uProjection = glGetUniformLocation(shader, "projection");
	GLuint uModelview = glGetUniformLocation(shader, "modelview");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &Window::V[0][0]);

  glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glDrawArrays(GL_LINE_STRIP, 0, points.size() * sizeof(glm::vec3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
