#pragma once
#ifndef _WATER_H_
#define _WATER_H_

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

class Water {
private:
	glm::mat4 toWorld;
	
	// IDs
	GLuint VBO, VAO, NBO, TBO, EBO;
	GLuint reflect_FBO, reflect_texture, reflect_DBO;	// For reflection frame buffer
	GLuint refract_FBO, refract_texture, refract_DTO;	// For refraction frame buffer
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

	void init_reflection_buff();		// Initialize buffer for storing water reflection texture
	void init_refraction_buff();		// Initialize buffer for storing water refraction texture
	void bind_FBO(int frameBuffer);		// Bind reflection or refraction FBO

public:
	Water();
	~Water();

	void loadWaterGrid();	// Triangular grid loading along with its vertices, normals
	void genIndexBuff();	// Create indices for grid
	void genNormals();		// Create normals for grid

	void init_buff();		// Main buffers for water drawing
	void draw(GLuint);		// Draw water

	/* Frame buffer code*/
	void init_FBOs();
	void bind_reflect_FBO();
	void bind_refract_FBO();
	void unbind_FBO();	// Unbind reflection/refraction FBO
};

#endif