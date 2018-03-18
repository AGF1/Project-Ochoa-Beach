#include "Water.h"
#include "Window.h"

#define REFLECT_WIDTH 320
#define REFLECT_HEIGHT 180

#define REFRACT_WIDTH 1280
#define REFRACT_HEIGHT 720

Water::Water() {
	toWorld = glm::mat4(1.0f);
	loadWaterGrid();
}

Water::~Water() {
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	//glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &EBO);
	//glDeleteTextures(1, &textureID);

	// Clean FBOs and their attachments
	glDeleteFramebuffers(1, &reflect_FBO);
	glDeleteTextures(1, &reflect_texture);
	glDeleteRenderbuffers(1, &reflect_DBO);
	glDeleteFramebuffers(1, &refract_FBO);
	glDeleteTextures(1, &refract_texture);
	glDeleteTextures(1, &refract_DTO);
}

/*-------------------------BUFFER CREATION CODE------------------*/
void Water::loadWaterGrid() {
	int width = 75;
	int height = 75;

	// Resize buffers
	int num_vertices = width * height;
	vertices.resize(num_vertices);
	normals.resize(num_vertices);
	//tex_coords.resize(num_vertices);
	indices.resize(num_vertices);

	// Get scaled terrain dimensions in world dimensions strectched out to a 1000 by 1000 flat square
	float terrWidth = (width - 1) * 20;
	float terrHeight = (height - 1) * 20;
	float centerTerrWidth = terrWidth * 0.5f;
	float centerTerrHeight = terrHeight * 0.5f;

	// load up buffers with height data
	for (unsigned int j = 0; j < height; j++) {
		for (unsigned int i = 0; i < width; i++) {
			unsigned int index = (j * width) + i;
			
			// Texture coordinates
			float tex_s = (i / (float)(width - 1));
			float tex_t = (j / (float)(height - 1));

			// Vertex coordinates
			float x = (tex_s * terrWidth) - centerTerrWidth;
			float y = -6.0f;
			float z = (tex_t * terrHeight) - centerTerrHeight;

			normals[index] = glm::vec3(0);	// Fill out later
			vertices[index] = glm::vec3(x, y, z);
			//tex_coords[index] = glm::vec2(tex_s, tex_t);
		}
	}
	// Create and load buffers
	genIndexBuff();
	genNormals();
	init_buff();
}   

void Water::genIndexBuff() {
	const unsigned int width = 75;
	const unsigned int height = 75;

	// Two triangles per quad
	const unsigned int numTriangles = (width - 1) * (height - 1) * 2;

	// Each triangle has three vertices
	indices.resize(numTriangles * 3);

	unsigned int indices_i = 0;	// Indices buffer index
	for (unsigned int j = 0; j < (height - 1); j++) {
		for (unsigned int i = 0; i < (width - 1); i++) {
			int vert_i = (j * width) + i;
			// Right triangle of Quad
			indices[indices_i++] = vert_i;
			indices[indices_i++] = vert_i + width + 1;
			indices[indices_i++] = vert_i + 1;
			// Left triangle of Quad
			indices[indices_i++] = vert_i;
			indices[indices_i++] = vert_i + width;
			indices[indices_i++] = vert_i + width + 1;
		}
	}
}

void Water::genNormals() {
	// Get triangle vertices
	for (unsigned int i = 0; i < indices.size(); i += 3) {
		glm::vec3 v0 = vertices[indices[i + 0]];
		glm::vec3 v1 = vertices[indices[i + 1]];
		glm::vec3 v2 = vertices[indices[i + 2]];

		// Use cross product to get normals
		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		// Add normals of each neighboring triangle together
		normals[indices[i + 0]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}

	// Normalize all the normals
	for (unsigned int i = 0; i < normals.size(); ++i) { normals[i] = glm::normalize(normals[i]); }
}

void Water::init_buff() {
	// Create array object & buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	//glGenBuffers(1, &TBO);
	glGenBuffers(1, &EBO);

	// Bind vertex array buffer
	glBindVertexArray(VAO);

	// Bind vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind normal buffer object
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind texture coordinate buffer object
	/*
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(glm::vec2), tex_coords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	*/

	// Bind index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	glBindVertexArray(0);
}

void Water::draw(GLuint shaderProgram) {
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;

	// Establish variables for shader program
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	// Add clipping plane
	glUniform4f(glGetUniformLocation(shaderProgram, "plane"), 0.0f, 1.0f, 0.0f, -6.0f);

	// Draw Terrain
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "terrain"), 0);
	//glBindTexture(GL_TEXTURE_2D, textureID);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	// Deactivate and unbind skybox texture
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_TEXTURE0);
	//glDisable(GL_TEXTURE_2D);
}

void Water::init_FBOs() {
	init_reflection_buff();
	init_refraction_buff();
}

void Water::init_reflection_buff() {
	glGenFramebuffers(1, &reflect_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflect_FBO);

	// Create and bind reflection texture
	glGenTextures(1, &reflect_texture);
	glBindTexture(GL_TEXTURE_2D, reflect_texture);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECT_WIDTH, REFLECT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Add filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Create depth buffer
	glGenRenderbuffers(1, &reflect_DBO);
	glBindRenderbuffer(GL_RENDERBUFFER, reflect_DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, REFLECT_WIDTH, REFLECT_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflect_DBO);
	// Attach reflection texture as color attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflect_texture, 0);
	// Create framebuffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Detach framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, REFLECT_WIDTH, REFLECT_HEIGHT);
}

void Water::init_refraction_buff() {
	glGenFramebuffers(1, &refract_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refract_FBO);

	// Create and bind refraction texture
	glGenTextures(1, &refract_texture);
	glBindTexture(GL_TEXTURE_2D, refract_texture);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFRACT_WIDTH, REFRACT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Add filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refract_texture, 0);

	// Create depth texture attachment
	glGenTextures(1, &refract_DTO);
	glBindTexture(GL_TEXTURE_2D, refract_DTO);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACT_WIDTH, REFRACT_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// Add filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refract_DTO, 0);
	// Attach refraction texture as color attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refract_texture, 0);

	// Create framebuffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Detach framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, REFLECT_WIDTH, REFLECT_HEIGHT);
}

void Water::bind_reflect_FBO() {
	bind_FBO(reflect_FBO);
}

void Water::bind_refract_FBO() {
	bind_FBO(refract_FBO);
}

void Water::bind_FBO(int frameBuffer) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, REFLECT_WIDTH, REFLECT_HEIGHT);
}

void Water::unbind_FBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::width, Window::height);
}