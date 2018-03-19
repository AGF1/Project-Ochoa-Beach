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
private:
	glm::mat4 toWorld;
	const char * heightmap_path;

	// Scale size of terrain
	float xz_scale;
	float height_scale;
	float ground_translate;	// Move the ground down

	// Heightmap dimensions
	glm::vec2 hMapDimensions;

	// Buffer locations
	GLuint VBO, VAO, NBO, TBO, EBO;
	GLuint uProjection, uModelview, uView, uModel;
	GLuint textureID;

	// Rename buffer objects for ease of reading
	typedef std::vector<glm::vec3> PosBuff;
	//typedef std::vector<glm::vec4> ColorBuff;
	typedef std::vector<glm::vec3> NormBuff;
	typedef std::vector<glm::vec2> TexCoordBuff;
	typedef std::vector<GLuint> IndexBuff;

	// Buffers
	PosBuff vertices;
	//ColorBuff colorBuffer;
	NormBuff normals;
	TexCoordBuff tex_coords;
	IndexBuff indices;

public:
	Terrain();
	Terrain(float, float, float, const char *);
	~Terrain();

	void init_buffers();
	void genIndexBuff();
	void genNormals();
	unsigned char* loadPPM(const char* filename, int& width, int& height);
	void loadHeightmap();
	void loadTexture();
	void draw(GLuint);
};

#endif