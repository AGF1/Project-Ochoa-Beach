#include "Water.h"
#include "Window.h"

#define DUDV_PATH "../assets/textures/waterDUDV.png"
#define NORMAL_PATH "../assets/textures/normal.png"
#define WAVE_SPEED 0.002f

#define FORWARD true
#define BACKWARD false

Water::Water() {
	toWorld = glm::mat4(1.0f);
	water_level = -6.0f;
	move_factor = 0.0f;
	loadWaterGrid();
	loadMaps();
}

Water::Water(int water_level) {
	toWorld = glm::mat4(1.0f);
	this->water_level = water_level;
	move_factor = 0.0f;
	loadWaterGrid();
	loadMaps();
}

Water::~Water() {
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &EBO);

	// Clean FBOs and their attachments
	glDeleteFramebuffers(1, &reflect_FBO);
	glDeleteTextures(1, &reflect_texture);
	glDeleteRenderbuffers(1, &reflect_DBO);
	glDeleteFramebuffers(1, &refract_FBO);
	glDeleteTextures(1, &refract_texture);
	glDeleteTextures(1, &refract_DTO);

	// Delete other loaded textures
	glDeleteTextures(1, &dudvTextureID);
	glDeleteTextures(1, &normalTextureID);
	glDeleteTextures(1, &skyboxTextureID);

	// Empty buffers
	vertices.clear();
	normals.clear();
	tex_coords.clear();
	indices.clear();
}

float Water::getWaterLevel() { return water_level; }

/*-------------------------BUFFER CREATION CODE------------------*/
void Water::loadWaterGrid() {
	int width = 5;
	int height = 5;

	// Resize buffers
	int num_vertices = width * height;
	vertices.resize(num_vertices);
	normals.resize(num_vertices);
	tex_coords.resize(num_vertices);
	indices.resize(num_vertices);

	// Get scaled terrain dimensions in world dimensions strectched out to a 1000 by 1000 flat square
	float terrWidth = (width - 1) * (1000 / (width - 1));
	float terrHeight = (height - 1) * (1000 / (height - 1));
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
			float y = water_level;
			float z = (tex_t * terrHeight) - centerTerrHeight;

			normals[index] = glm::vec3(0);	// Fill out later
			vertices[index] = glm::vec3(x, y, z);
			tex_coords[index] = glm::vec2(tex_s, tex_t);
		}
	}
	// Create and load buffers
	genIndexBuff();
	genNormals();
	init_buff();
}   

void Water::genIndexBuff() {
	const unsigned int width = 5;
	const unsigned int height = 5;

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
	glGenBuffers(1, &TBO);
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
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(glm::vec2), tex_coords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);


	// Bind index buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	glBindVertexArray(0);
}

void Water::loadMaps() {
	std::cout << "loading DUDV_MAP..." << std::endl;
	loadTexture(DUDV_PATH, &dudvTextureID);
	std::cout << "loading NORMAL MAP..." << std::endl;
	loadTexture(NORMAL_PATH, &normalTextureID);
	std::cout << "loading CUBEMAP for reflection..." << std::endl;
	loadSkyboxTexture();
}

void Water::loadTexture(const char * filename, GLuint * textureID) {
	int twidth, theight, channels;
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, textureID);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, *textureID);

	// Some lighting/filtering settings
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Don't let bytes be padded
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	// set GL_MODULATE to mix texture with polygon color for shading

	tdata = SOIL_load_image(filename, &twidth, &theight, &channels, SOIL_LOAD_RGB);
	if (twidth < 0 || theight < 0) {
		std::cout << filename << " not loaded correctly!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SOIL_free_image_data(tdata);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Water::loadSkyboxTexture() {
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, &skyboxTextureID);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

	// Load image files
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		tdata = loadPPM(faces[i], twidth, theight);
		if (tdata == NULL) return;

		// Generate the texture
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 3, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
	}

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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

	// Add reflection and refraction texture locations to shader
	glUniform1i(glGetUniformLocation(shaderProgram, "reflect_texture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "refract_texture"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 4);

	// Add dudv_map to fragment shader
	glUniform1i(glGetUniformLocation(shaderProgram, "dudv_map"), 2);
	move_factor += WAVE_SPEED;	// Calculate wave speed and keep it from going above 1
	move_factor = fmod(move_factor, FLT_MAX - 1);
	//move_factor = glm::cos(move_factor);
	//move_factor = fmod(move_factor, 0.5f);

	glUniform1f(glGetUniformLocation(shaderProgram, "move_factor"), move_factor);

	// Add normal map to fragment shader
	glUniform1i(glGetUniformLocation(shaderProgram, "normal_map"), 3);
	glUniform3f(glGetUniformLocation(shaderProgram, "light_dir"), -0.3f, 0.2f, -1.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "light_color"), 1.0, 1.0, 1.0);

	// Add camera position for fresnel effect
	glUniform3fv(glGetUniformLocation(shaderProgram, "cam_pos"), 1, &Window::cam_pos[0]);

	// Draw Water
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflect_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refract_texture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvTextureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalTextureID);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	// Deactivate and unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE0);
	glDisable(GL_TEXTURE1);
	glDisable(GL_TEXTURE2);
	glDisable(GL_TEXTURE3);
	glDisable(GL_TEXTURE4);
}

void Water::init_FBOs() {
	init_reflection_buff();
	init_refraction_buff();
	unbind_FBO();
}

void Water::init_reflection_buff() {
	glGenFramebuffers(1, &reflect_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflect_FBO);

	// Create and bind reflection texture
	glGenTextures(1, &reflect_texture);
	glBindTexture(GL_TEXTURE_2D, reflect_texture);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Add filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Create depth buffer
	glGenRenderbuffers(1, &reflect_DBO);
	glBindRenderbuffer(GL_RENDERBUFFER, reflect_DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflect_DBO);
	// Attach reflection texture as color attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflect_texture, 0);
	// Create framebuffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Detach framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::width, Window::height);
}

void Water::init_refraction_buff() {
	glGenFramebuffers(1, &refract_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refract_FBO);

	// Create and bind refraction texture
	glGenTextures(1, &refract_texture);
	glBindTexture(GL_TEXTURE_2D, refract_texture);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Add filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refract_texture, 0);

	// Create depth texture attachment
	glGenTextures(1, &refract_DTO);
	glBindTexture(GL_TEXTURE_2D, refract_DTO);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Window::width, Window::height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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
	glViewport(0, 0, Window::width, Window::height);
}

void Water::bind_reflect_FBO() { bind_FBO(reflect_FBO); }

void Water::bind_refract_FBO() { bind_FBO(refract_FBO); }

void Water::bind_FBO(int frameBuffer) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, Window::width, Window::height);
}

void Water::unbind_FBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::width, Window::height);
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* Water::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}

void Water::resize_FBOs() {
	// Resize Reflection FBO
	glBindFramebuffer(GL_FRAMEBUFFER, reflect_FBO);
	glBindTexture(GL_TEXTURE_2D, reflect_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, reflect_DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height);

	// Resize Refraction FBO (did not resize depth texture object)
	glBindFramebuffer(GL_FRAMEBUFFER, refract_FBO);
	glBindTexture(GL_TEXTURE_2D, refract_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, refract_DTO);
	// Add empty texture for now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Window::width, Window::height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}