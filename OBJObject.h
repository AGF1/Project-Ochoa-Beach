#pragma once
#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:
	std::vector<unsigned int> indices;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	glm::mat4 toWorld;
	glm::mat4 origPos;
	glm::mat4 toCenter;
	glm::mat4 toScale;
	float angle;
	float scaleOffset;
	float scale;
	float xOffset;
	float yOffset;
	float zOffset;
	float x;
	float y;
	float z;
	char rotateDir;

public:
	OBJObject(const char* filepath);
	OBJObject(const char *filepath, float scale, float xOffset, float yOffset, float zOffset, char rotateDir);
	~OBJObject();

	void parse(const char* filepath);
	void init();
	void draw(GLuint, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 lightDir, glm::vec3 pLightColor, glm::vec3 pLightPos, glm::vec3 sLightColor, glm::vec3 sLightPos, glm::vec3 sLightAim, glm::vec2 sLightParams, glm::vec3 camPos, glm::vec4 materialParams);
	void draw(GLuint, glm::mat4 modelview);
	void update();
	void spin(float deg);
	void move(float x, float y, float z);
	void resize(float amt);
	void rotate(float x, float y, float z, float angle);
	void rotateOrigin(float x, float y, float z, float angle);
	void resetPos();
	void resetRot();
	void resetScale();

	glm::vec3 getPosition();

	// These variables are needed for the shader program
	GLuint VBO[2], VAO, EBO;
	GLuint uProjection, uModel, uView, uObjColor, uLightColor, uLightDir,
		uPLightColor, uPLightPos, uSLightColor, uSLightPos, uSLightAim,
		uSLightCut, uSLightExp, uCamPos, uAmb, uDif, uSpec, uShine, uMode,
		uModelView;
};

#endif