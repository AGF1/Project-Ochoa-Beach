#include "Terrain.h"
#include "Window.h"

#include "soil.h"	// Load in heightmap data using these features

#define TEXTURE_PATH "../assets/textures/sand2.ppm"
#define HEIGHTMAP_PATH "../assets/SanDiegoTerrain.jpg"

// Default constructor with set scales and heightmap
Terrain::Terrain() {
	toWorld = glm::mat4(1.0f);
	xz_scale = 0.5f;
	height_scale = 10.0f;
	ground_translate = -9.0f;
	heightmap_path = "../assets/SanDiegoTerrain.jpg";
	
	loadHeightmap();
	loadTexture();
}

// Constructor that controls scaling, ground level, and heightmap path
Terrain::Terrain(float xz_scale, float height_scale, float ground_translate, const char * hmPath) {
	toWorld = glm::mat4(1.0f);
	this->xz_scale = xz_scale;
	this->height_scale = height_scale;
	this->ground_translate = ground_translate;
	heightmap_path = hmPath;

	loadHeightmap();
	loadTexture();
}

Terrain::~Terrain() {
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &textureID);
}

void Terrain::init_buffers() {
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

void Terrain::genIndexBuff() {
	const unsigned int terrWidth = hMapDimensions.x;
	const unsigned int terrHeight = hMapDimensions.y;

	// Two triangles per quad
	const unsigned int numTriangles = (terrWidth - 1) * (terrHeight - 1) * 2;
	//const unsigned int numTriangles = (terrWidth) * (terrHeight) * 2;

	// Each triangle has three vertices
	indices.resize(numTriangles * 3);

	unsigned int indices_i = 0;	// Indices buffer index
	for (unsigned int j = 0; j < (terrHeight - 1); j++) {
		for (unsigned int i = 0; i < (terrWidth - 1); i++) {
			int vert_i = (j * terrWidth) + i;
			// Right triangle of Quad
			indices[indices_i++] = vert_i;
			indices[indices_i++] = vert_i + terrWidth + 1;
			indices[indices_i++] = vert_i + 1;
			// Left triangle of Quad
			indices[indices_i++] = vert_i;
			indices[indices_i++] = vert_i + terrWidth;
			indices[indices_i++] = vert_i + terrWidth + 1;
		}
	}
}

void Terrain::genNormals() {
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

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* Terrain::loadPPM(const char* filename, int& width, int& height)
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

void Terrain::loadHeightmap() {
	int map_width, map_height, channels;	
	
	// Get Terrain data and dimensions
	unsigned char * hmData = SOIL_load_image(heightmap_path, &map_width, &map_height, &channels, SOIL_LOAD_L);
	if (map_width < 0) { std::cout << "Heightmap not loading correctly!" << std::endl; return; }

	// Resize buffers
	int num_vertices = map_width * map_height;
	vertices.resize(num_vertices);
	normals.resize(num_vertices);
	tex_coords.resize(num_vertices);
	indices.resize(num_vertices);

	// Store size of heightmap
	hMapDimensions = glm::vec2(map_width, map_height);

	// Get scaled terrain dimensions in world dimensions
	float terrWidth = 1000.0;
	float terrHeight = 1000.0;
	float centerTerrWidth = terrWidth * 0.5f;
	float centerTerrHeight = terrHeight * 0.5f;

	// load up buffers with height data
	for (unsigned int j = 0; j < map_height; j++) {
		for (unsigned int i = 0; i < map_width; i++) {
			unsigned int index = (j * map_width) + i;
			float heightValue = hmData[index] / 255.0f;	// Get scaled height (between 0 and 1)

			// Texture coordinates
			float tex_s = (i / (float)(map_width - 1));
			float tex_t = (j / (float)(map_height - 1));

			// Vertex coordinates
			float x = (tex_s * terrWidth) - centerTerrWidth;
			float y = (heightValue * height_scale) + ground_translate;
			float z = (tex_t * terrHeight) - centerTerrHeight;

			normals[index] = glm::vec3(0);	// Fill out later
			vertices[index] = glm::vec3(x, y, z);
			tex_coords[index] = glm::vec2(tex_s, tex_t);
		}
	}
	// Free up heightmap data
	SOIL_free_image_data(hmData);

	// Create and load buffers
	genIndexBuff();
	genNormals();
	init_buffers();

	std::cout << "Heightmap loaded" << std::endl;
}

void Terrain::loadTexture() {
	int twidth, theight, channels;
	unsigned char* tdata;  // texture pixel data

	// Create ID for texture
	glGenTextures(1, &textureID);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Some lighting/filtering settings
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Don't let bytes be padded
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	// set GL_MODULATE to mix texture with polygon color for shading

	tdata = loadPPM(TEXTURE_PATH, twidth, theight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Terrain::draw(GLuint shaderProgram) {
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

	// Send clipping plane to relevant shaders
	glUniform4f(glGetUniformLocation(shaderProgram, "plane"), 0.0, Window::plane_vec_dir, 0.0, Window::water_level);

	// Draw Terrain
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "terrain"), 0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	// Deactivate and unbind skybox texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);

}

