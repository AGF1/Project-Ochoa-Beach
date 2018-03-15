#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

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

class Terrain {
	glm::mat4 toWorld;

	float xz_scale;
	float height_scale;

	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview, uView, uMode;
	GLuint textureID;

public:
	Terrain();
	Terrain(float, float);
	~Terrain();

	void init_buffers();
	unsigned char* loadPPM(const char* filename, int& width, int& height);
	void loadTexture();
	void draw(GLuint);
};

// Hardcoded floor for checking if texture code works
/*
const GLfloat vertices_test[4][3] = {
	// "Front" vertices
	{ 200.0,  200.0,  200.0 },{ -200.0,  200.0, 200.0 },
	// "Back" vertices
	{ 200.0,  200.0, -200.0 },{ -200.0,  200.0, -200.0 }
};

const GLuint indices_test[6] = {
	 1, 0, 2, 3
};
*/
const GLfloat vertices_test[8][3] = {
	// "Front" vertices
	{ -2.0, -2.0,  2.0 },{ 2.0, -2.0,  2.0 },{ 2.0,  2.0,  2.0 },{ -2.0,  2.0, 2.0 },
	// "Back" vertices
{ -2.0, -2.0, -2.0 },{ 2.0, -2.0, -2.0 },{ 2.0,  2.0, -2.0 },{ -2.0,  2.0, -2.0 }
};

// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
const GLuint indices_test[6][6] = {
	// Front face
	{ 0, 1, 2, 2, 3, 0 },
	// Top face
{ 1, 5, 6, 6, 2, 1 },
// Back face
{ 7, 6, 5, 5, 4, 7 },
// Bottom face
{ 4, 0, 3, 3, 7, 4 },
// Left face
{ 4, 5, 1, 1, 0, 4 },
// Right face
{ 3, 2, 6, 6, 7, 3 }
};


#endif