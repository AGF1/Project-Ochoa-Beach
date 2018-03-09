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
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	FILE* objFile = fopen(filepath, "rb");
	if (objFile == NULL)
	{
		exit(-1);
	}
	float xMax = LONG_MIN;
	float xMin = LONG_MAX;
	float yMax = LONG_MIN;
	float yMin = LONG_MAX;
	float zMax = LONG_MIN;
	float zMin = LONG_MAX;
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
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
			}
			else if (c2 == ' ')
			{
				GLfloat x, y, z;
				//Vertices (v) have x, y, z coordinates
				fscanf(objFile, "%f %f %f", &x, &y, &z);
				if (x > xMax) xMax = x;
				if (x < xMin) xMin = x;
				if (y > yMax) yMax = y;
				if (y < yMin) yMin = y;
				if (z > zMax) zMax = z;
				if (z < zMin) zMin = z;
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}
		}
		else if (c1 == 'f')
		{
			if (fgetc(objFile) != ' ') continue;
			unsigned int v1, v2, v3, vn1, vn2, vn3;
			fscanf(objFile, "%u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
			v1 -= 1;
			v2 -= 1;
			v3 -= 1;
			//vn1 -= 1;
			//vn2 -= 1;
			//vn3 -= 1;
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);
			//indices.push_back(vn1);
			//indices.push_back(vn2);
			//indices.push_back(vn3);
		}
		else
		{
		}
	}
	fclose(objFile);
	float xDif = xMax - xMin;
	float yDif = yMax - yMin;
	float zDif = zMax - zMin;
	float scaleBy;
	if (xDif > yDif)
	{
		if (xDif > zDif)
		{
			scaleBy = xDif;
		}
		else
		{
			scaleBy = zDif;
		}
	}
	else if (yDif > zDif)
	{
		scaleBy = yDif;
	}
	else
	{
		scaleBy = zDif;
	}
	scaleBy = 2 / scaleBy;
	float xCen = (xMax + xMin) / 2;
	float yCen = (yMax + yMin) / 2;
	float zCen = (zMax + zMin) / 2;
	toCenter = glm::translate(glm::mat4(1.0f), glm::vec3(-xCen, -yCen, -zCen));
	toScale = glm::scale(glm::mat4(1.0f), glm::vec3(scaleBy, scaleBy, scaleBy));
	origPos = toScale * toCenter;
	xOffset -= xCen;
	yOffset -= yCen;
	zOffset -= zCen;
	scaleOffset *= scaleBy;
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

void OBJObject::draw(GLuint shaderProgram, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 lightDir, glm::vec3 pLightColor, glm::vec3 pLightPos, glm::vec3 sLightColor, glm::vec3 sLightPos, glm::vec3 sLightAim, glm::vec2 sLightParams, glm::vec3 camPos, glm::vec4 materialParams)
{
	//Material Params: ambient, diffuse, specular, shininess
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 model = toWorld * origPos;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");

	uObjColor = glGetUniformLocation(shaderProgram, "objectColor");
	uLightColor = glGetUniformLocation(shaderProgram, "lightColor");
	uLightDir = glGetUniformLocation(shaderProgram, "lightDir");
	uPLightColor = glGetUniformLocation(shaderProgram, "pLightColor");
	uPLightPos = glGetUniformLocation(shaderProgram, "pLightPos");
	uSLightColor = glGetUniformLocation(shaderProgram, "sLightColor");
	uSLightPos = glGetUniformLocation(shaderProgram, "sLightPos");
	uSLightAim = glGetUniformLocation(shaderProgram, "sLightAim");
	uSLightCut = glGetUniformLocation(shaderProgram, "sLightCut");
	uSLightExp = glGetUniformLocation(shaderProgram, "sLightExp");
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
	glUniform3fv(uObjColor, 1, &(objColor.x));
	glUniform3fv(uLightColor, 1, &(lightColor.x));
	glUniform3fv(uLightDir, 1, &(lightDir.x));
	glUniform3fv(uPLightColor, 1, &(pLightColor.x));
	glUniform3fv(uPLightPos, 1, &(pLightPos.x));
	glUniform3fv(uSLightColor, 1, &(sLightColor.x));
	glUniform3fv(uSLightPos, 1, &(sLightPos.x));
	glUniform3fv(uSLightAim, 1, &(sLightAim.x));
	glUniform1fv(uSLightCut, 1, &(sLightParams.x));
	glUniform1fv(uSLightExp, 1, &(sLightParams.y));
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

void OBJObject::draw(GLuint shaderProgram, glm::mat4 modelview)
{
	// Calculate the combination of the model and view (camera inverse) matrices
	//glm::mat4 model = toWorld * origPos;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	//uProjection = glGetUniformLocation(shaderProgram, "projection");
	//uModel = glGetUniformLocation(shaderProgram, "model");
	//uView = glGetUniformLocation(shaderProgram, "view");

	//uObjColor = glGetUniformLocation(shaderProgram, "objectColor");
	//uLightColor = glGetUniformLocation(shaderProgram, "lightColor");
	//uLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	//uCamPos = glGetUniformLocation(shaderProgram, "camPos");

	//uDif = glGetUniformLocation(shaderProgram, "diffuseModifier");
	//uSpec = glGetUniformLocation(shaderProgram, "specularModifier");

	uMode = glGetUniformLocation(shaderProgram, "mode");
	uModelView = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	//glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
	glUniformMatrix4fv(uModelView, 1, GL_FALSE, &modelview[0][0]);
	//glUniform3fv(uObjColor, 1, &(objColor.x));
	//glUniform3fv(uLightColor, 1, &(lightColor.x));
	//glUniform3fv(uLightPos, 1, &(lightPos.x));
	//glUniform3fv(uCamPos, 1, &(camPos.x));
	//glUniform1fv(uDif, 1, &diffuse);
	//glUniform1fv(uSpec, 1, &specular);
	glUniform1i(uMode, 1);
	// Now draw the object. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using the number of indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size() * sizeof(int), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::update()
{
	//spin(1.0f);
}

void OBJObject::spin(float deg)
{
	angle += deg;
	if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
	// This creates the matrix to rotate the object
	toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	if (rotateDir == 'x') {
		toWorld = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)) * toWorld;
	}
	else if (rotateDir == 'y') {
		toWorld = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * toWorld;
	}
	else if (rotateDir == 'z') {
		toWorld = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * toWorld;
	}
	else
	{
	}
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * toWorld;
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