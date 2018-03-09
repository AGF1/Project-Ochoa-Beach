#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

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
#include "OBJObject.h"
#include "Window.h"

class Geometry : public Node
{
private:
	glm::mat4 modelview;
	OBJObject* object;
	GLint shaderprogram;

public:
	Geometry(GLint shaderprogram);
	~Geometry();
	void init(char* filename);
	void draw(glm::mat4 C);
	void update();
};

#endif