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

const GLfloat vertices_test[4][3] = {
	// "Front" vertices
	{ 200.0,  200.0,  200.0 },{ -200.0,  200.0, 200.0 },
	// "Back" vertices
	{ 200.0,  200.0, -200.0 },{ -200.0,  200.0, -200.0 }
};

const GLuint indices_test[6] = {
	 1, 0, 2, 3
};

#endif