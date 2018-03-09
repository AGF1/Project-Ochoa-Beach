#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <list>

#include "Node.h"

class Transform : public Node
{
private:
	glm::mat4 M;
	glm::mat4 A;
	std::list<Node*> children;
	bool animated;
	float angle;

public:
	Transform(glm::mat4 M, bool animated);
	~Transform();
	void addChild(Node* child);
	void removeChild(Node* child);
	void draw(glm::mat4 C);
	void update();
};

#endif