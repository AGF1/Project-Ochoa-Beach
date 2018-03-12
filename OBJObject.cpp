#include "OBJObject.h"
#include "Window.h"
#include <vector>

OBJObject::OBJObject(const char *filepath)
{
	toWorld = glm::mat4(1.0f);
	origPos = toWorld;
	this->scaleOffset = 1.0f;
	this->xOffset = 0.0f;
	this->yOffset = 0.0f;
	this->zOffset = 0.0f;
	this->rotateDir = ' ';
	parse(filepath);
	this->angle = 0.0f;
	this->scale = 1.0f;
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	init();
}

OBJObject::OBJObject(const char *filepath, float scale, float xOffset, float yOffset, float zOffset, char rotateDir)
{
	toWorld = glm::mat4(1.0f);
	this->scaleOffset = scale;
	this->xOffset = xOffset;
	this->yOffset = yOffset;
	this->zOffset = zOffset;
	this->rotateDir = rotateDir;
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset, yOffset, zOffset)) * toWorld;
	origPos = toWorld;
	parse(filepath);
	this->angle = 0.0f;
	this->scale = scale;
	this->x = xOffset;
	this->y = yOffset;
	this->z = zOffset;
	init();
}

OBJObject::~OBJObject()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);
}

void OBJObject::parse(const char *filepath)
{
	float xMax = LONG_MIN;
	float xMin = LONG_MAX;
	float yMax = LONG_MIN;
	float yMin = LONG_MAX;
	float zMax = LONG_MIN;
	float zMin = LONG_MAX;
	std::vector<glm::vec3> rawVertices;
	std::vector<glm::vec3> rawNormals;
	std::vector<std::pair<unsigned int, unsigned int>> rawIndices;
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	FILE* objFile = fopen(filepath, "rb");
	if (objFile == NULL)
	{
		exit(-1);
	}
	while (!feof(objFile))
	{
		char c1 = fgetc(objFile);
		if (c1 != 'v' && c1 != 'f') continue;
		if (c1 == 'v')
		{
			char c2 = fgetc(objFile);
			if (c2 == 'n')
			{
				GLfloat x, y, z;
				//Vertex normals (vn) have x, y, z coordinates
				fscanf(objFile, "%f %f %f", &x, &y, &z);
				rawNormals.push_back(glm::vec3(x, y, z));
			}
			else if (c2 == ' ')
			{
				GLfloat x, y, z;
				//Vertices (v) have x, y, z coordinates
				fscanf(objFile, "%f %f %f", &x, &y, &z);
				rawVertices.push_back(glm::vec3(x, y, z));
				if (x > xMax) xMax = x;
				if (x < xMin) xMin = x;
				if (y > yMax) yMax = y;
				if (y < yMin) yMin = y;
				if (z > zMax) zMax = z;
				if (z < zMin) zMin = z;
			}
		}
		else if (c1 == 'f')
		{
			if (fgetc(objFile) != ' ') continue;
			unsigned int v1, v2, v3, vn1, vn2, vn3, vt1, vt2, vt3;
			fscanf(objFile, "%u/%u/%u %u/%u/%u %u/%u/%u", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
			v1 -= 1;
			v2 -= 1;
			v3 -= 1;
			vn1 -= 1;
			vn2 -= 1;
			vn3 -= 1;
			rawIndices.push_back(std::pair<unsigned int, unsigned int>(v1, vn1));
			rawIndices.push_back(std::pair<unsigned int, unsigned int>(v2, vn2));
			rawIndices.push_back(std::pair<unsigned int, unsigned int>(v3, vn3));
		}
		else
		{
		}
	}
	fclose(objFile);
	for (unsigned int i = 0; i < rawIndices.size(); i++)
	{
		indices.push_back(i);
		vertices.push_back(rawVertices[rawIndices[i].first].x);
		vertices.push_back(rawVertices[rawIndices[i].first].y);
		vertices.push_back(rawVertices[rawIndices[i].first].z);
		normals.push_back(rawNormals[rawIndices[i].second].x);
		normals.push_back(rawNormals[rawIndices[i].second].y);
		normals.push_back(rawNormals[rawIndices[i].second].z);
	}
	float xDist = xMax - xMin;
	float yDist = yMax - yMin;
	float zDist = zMax - zMin;
	float xCenter = xMin + (xDist / 2.0f);
	float yCenter = yMin + (yDist / 2.0f);
	float zCenter = zMin + (zDist / 2.0f);
	float maxDist = glm::max(xDist, glm::max(yDist, zDist));
	origPos = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f / maxDist, 10.0f / maxDist, 10.0f / maxDist)) * glm::translate(glm::mat4(1.0f), glm::vec3(-xCenter, -yCenter, -zCenter));
}

void OBJObject::init()
{
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &(vertices[0]), GL_STATIC_DRAW);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), &(indices[0]), GL_STATIC_DRAW);

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

void OBJObject::draw(GLuint shaderProgram, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 lightDir, glm::vec3 camPos, glm::vec4 materialParams)
{
	//Material Params: ambient, diffuse, specular, shininess
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 model = toWorld * origPos;
	glm::mat4 modelview = Window::V * model;
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

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
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
	// Now draw the object. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using the number of indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size()*sizeof(int), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::update()
{
}

void OBJObject::move(float x, float y, float z)
{
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * toWorld;
	this->x += x;
	this->y += y;
	this->z += z;
}

void OBJObject::resize(float amt)
{
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, this->y, this->z))
		* glm::scale(glm::mat4(1.0f), glm::vec3(amt, amt, amt))
		* glm::translate(glm::mat4(1.0f), glm::vec3(-this->x, -this->y, -this->z)) 
		* toWorld;
	scale *= amt;
}

void OBJObject::rotate(float x, float y, float z, float angle)
{
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, this->y, this->z))
		* glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale))
		* glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z))
		* glm::scale(glm::mat4(1.0f), glm::vec3(1 / scale, 1 / scale, 1 / scale))
		* glm::translate(glm::mat4(1.0f), glm::vec3(-this->x, -this->y, -this->z))
		* toWorld;
}

void OBJObject::rotateOrigin(float x, float y, float z, float angle)
{
	toWorld = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z)) * toWorld;
	glm::vec4 position = toWorld * glm::vec4(x, y, z, 1.0f);
	this->x = position.x;
	this->y = position.y;
	this->z = position.z;
}

void OBJObject::resetPos()
{
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, -z)) * toWorld;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

void OBJObject::resetRot()
{
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, -z)) * toWorld;
	toWorld = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * toWorld;
	angle = 0.0f;
}

void OBJObject::resetScale()
{
	toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * toWorld;
	scale = scaleOffset;
}

glm::vec3 OBJObject::getPosition()
{
	return glm::vec3(x, y, z);
}