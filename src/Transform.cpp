#include "Transform.h"

Transform::Transform(glm::mat4 m) {
  M = m;
}

void Transform::draw(glm::mat4 c, bool blur) {
  for (Node *child : children) {
    child->draw(c * M, blur);
  }
}

void Transform::addChild(Node *child) {
  children.push_back(child);
}

void Transform::removeChild(Node *child) {
  children.remove(child);
}

void Transform::update() {
  for (Node *child : children) {
    child->update();
  }
}
