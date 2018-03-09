#pragma once
#ifndef NODE_H
#define NODE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Node
{
public:
	virtual void draw(glm::mat4 C) = 0;
	virtual void update() = 0;
};

#endif