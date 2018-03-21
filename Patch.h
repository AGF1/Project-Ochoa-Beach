#ifndef _PATCH_H_
#define _PATCH_H_

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

class Patch
{
public:
	Patch(glm::vec3 position, glm::vec3 pts[16]);
	~Patch();

	void reinitialize(bool simple);
	void draw(GLuint, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 lightDir, glm::vec3 camPos, glm::vec4 materialParams, bool toon, bool simple);
	static glm::vec3 genSingleCurvePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	static glm::vec3 genSingleCurveTangent(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	static std::pair<glm::vec3, glm::vec3> genSinglePatchPoint(float u, float v, glm::vec3 pts[16]);
	static std::vector<glm::vec3> genCurvePoints(glm::vec3 start, glm::vec3 cp1, glm::vec3 cp2, glm::vec3 end, int numPoints);
	static std::vector<glm::vec3> genPatchPoints(glm::vec3 pts[16], int pointsPerCurve);

	glm::mat4 toWorld;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLuint> indices;
	std::vector<GLfloat> simpleVerts;
	std::vector<GLfloat> simpleNorms;
	std::vector<GLuint> simpleIndices;

	// These variables are needed for the shader program
	GLuint VBO[2], VAO, EBO;
	GLuint uProjection, uModel, uView, uObjColor, uLightColor, uLightDir, uCamPos, uAmb, uDif, uSpec, uShine, uMode, uModelView, uToon;
};

#endif

