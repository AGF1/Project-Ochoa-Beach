#ifndef _CURVE_H_
#define _CURVE_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Curve
{
public:
	Curve(GLfloat p[]);
	~Curve();

	void draw(GLuint);

	glm::mat4 toWorld;
	GLfloat points[2250];
	GLuint indices[750];

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview, uView, uMode;
};

#endif

