#include "Patch.h"
#include "Window.h"

Patch::Patch(glm::vec3 pts[16])
{
	std::vector<glm::vec3> points = genPatchPoints(pts, 7);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			vertices.push_back(points[(j * 14) + (2 * i)].x);
			vertices.push_back(points[(j * 14) + (2 * i)].y);
			vertices.push_back(points[(j * 14) + (2 * i)].z);
			normals.push_back(points[(j * 14) + (2 * i) + 1].x);
			normals.push_back(points[(j * 14) + (2 * i) + 1].y);
			normals.push_back(points[(j * 14) + (2 * i) + 1].z);
			vertices.push_back(points[(j * 14) + (2 * (i + 1))].x);
			vertices.push_back(points[(j * 14) + (2 * (i + 1))].y);
			vertices.push_back(points[(j * 14) + (2 * (i + 1))].z);
			normals.push_back(points[(j * 14) + (2 * (i + 1)) + 1].x);
			normals.push_back(points[(j * 14) + (2 * (i + 1)) + 1].y);
			normals.push_back(points[(j * 14) + (2 * (i + 1)) + 1].z);
		}
	}

	for (int i = 0; i < 98; i++)
	{
		indices.push_back(i);
	}

	toWorld = glm::mat4(1.0f);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);
	
	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	// 1: x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
	// 2: how many components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
	// 3: What type these components are
	// 4: GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
	// 5: Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
	// 6: Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &(indices[0]), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &(normals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &(indices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

Patch::~Patch()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);
}

void Patch::draw(GLuint shaderProgram, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 lightDir, glm::vec3 camPos, glm::vec4 materialParams, bool toon)
{ 
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");
	uModelView = glGetUniformLocation(shaderProgram, "modelview");

	uObjColor = glGetUniformLocation(shaderProgram, "objectColor");
	uLightColor = glGetUniformLocation(shaderProgram, "lightColor");
	uLightDir = glGetUniformLocation(shaderProgram, "lightDir");
	uCamPos = glGetUniformLocation(shaderProgram, "camPos");

	uAmb = glGetUniformLocation(shaderProgram, "ambientModifier");
	uDif = glGetUniformLocation(shaderProgram, "diffuseModifier");
	uSpec = glGetUniformLocation(shaderProgram, "specularModifier");
	uShine = glGetUniformLocation(shaderProgram, "shininess");

	uMode = glGetUniformLocation(shaderProgram, "mode");

	uToon = glGetUniformLocation(shaderProgram, "toon");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
	glUniformMatrix4fv(uModelView, 1, GL_FALSE, &modelview[0][0]);
	glUniform3fv(uObjColor, 1, &(objColor.x));
	glUniform3fv(uLightColor, 1, &(lightColor.x));
	glUniform3fv(uLightDir, 1, &(lightDir.x));
	glUniform3fv(uCamPos, 1, &(camPos.x));
	glUniform1fv(uAmb, 1, &materialParams.x);
	glUniform1fv(uDif, 1, &materialParams.y);
	glUniform1fv(uSpec, 1, &materialParams.z);
	glUniform1fv(uShine, 1, &materialParams.w);
	glUniform1i(uMode, 0);
	glUniform1i(uToon, toon);

	glDisable(GL_CULL_FACE);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with lines, using 49 indices, the type of the indices, and the offset to start from
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 14, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 28, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 42, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 56, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 70, 14);
	glDrawArrays(GL_TRIANGLE_STRIP, 84, 14);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

glm::vec3 Patch::genSingleCurvePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	//(1-t)^3 * p0 + 3 * (1-t)^2 * t * p1 + 3 * (1-t) * t^2 * p2 + t^3 * p3
	return glm::pow(1 - t, 3) * p0 + 3 * glm::pow(1 - t, 2) * t * p1 + 3 * (1 - t) * glm::pow(t, 2) * p2 + glm::pow(t, 3) * p3;
}

glm::vec3 Patch::genSingleCurveTangent(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	//(1-t)^3 * p0 + 3 * (1-t)^2 * t * p1 + 3 * (1-t) * t^2 * p2 + t^3 * p3
	return -3 * glm::pow(1 - t, 2) * p0 + 3 * glm::pow(1 - t, 2) * p1 - 6 * t * (1 - t) * p1 + 6 * t * (1 - t) * p2 - 3 * glm::pow(t, 2) * p2 + 3 * glm::pow(t, 2) * p3;
}

std::pair<glm::vec3, glm::vec3> Patch::genSinglePatchPoint(float u, float v, glm::vec3 pts[16])
{
	glm::vec3 q0 = genSingleCurvePoint(u, pts[0], pts[1], pts[2], pts[3]);
	glm::vec3 q1 = genSingleCurvePoint(u, pts[4], pts[5], pts[6], pts[7]);
	glm::vec3 q2 = genSingleCurvePoint(u, pts[8], pts[9], pts[10], pts[11]);
	glm::vec3 q3 = genSingleCurvePoint(u, pts[12], pts[13], pts[14], pts[15]);
	glm::vec3 r0 = genSingleCurvePoint(v, pts[0], pts[4], pts[8], pts[12]);
	glm::vec3 r1 = genSingleCurvePoint(v, pts[1], pts[5], pts[9], pts[13]);
	glm::vec3 r2 = genSingleCurvePoint(v, pts[2], pts[6], pts[10], pts[14]);
	glm::vec3 r3 = genSingleCurvePoint(v, pts[3], pts[7], pts[11], pts[15]);
	glm::vec3 x = genSingleCurvePoint(v, q0, q1, q2, q3);
	glm::vec3 tanv = genSingleCurveTangent(v, q0, q1, q2, q3);
	glm::vec3 tanu = genSingleCurveTangent(u, r0, r1, r2, r3);
	glm::vec3 normal = glm::cross(tanu, tanv);
	return std::pair<glm::vec3, glm::vec3>(x, normal);
}

std::vector<glm::vec3> Patch::genCurvePoints(glm::vec3 start, glm::vec3 cp1, glm::vec3 cp2, glm::vec3 end, int numPoints)
{
	std::vector<glm::vec3> points;
	if (numPoints < 2) return points;

	float inc = 1 / (numPoints - 1.0f);
	for (float t = 0; t <= 1; t += inc)
	{
		points.push_back(genSingleCurvePoint(t, start, cp1, cp2, end));
	}
	return points;
}

std::vector<glm::vec3> Patch::genPatchPoints(glm::vec3 pts[16], int pointsPerCurve)
{
	std::vector<glm::vec3> points;
	if (pointsPerCurve < 2) return points;

	float inc = 1 / (pointsPerCurve - 1.0f);
	for (float u = 0; u <= 1; u += inc)
	{
		for (float v = 0; v <= 1; v += inc)
		{
			std::pair<glm::vec3, glm::vec3> data = genSinglePatchPoint(u, v, pts);
			points.push_back(data.first);
			points.push_back(data.second);
		}
	}
	return points;
}
