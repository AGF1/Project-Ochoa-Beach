#include "Window.h"

const char* window_title = "GLFW Starter Project";
Cube * skybox;
OBJObject* anchor;
OBJObject* beachball;
OBJObject* chair;
OBJObject* crab;
OBJObject* hut;
OBJObject* chair2;
OBJObject* rock;
OBJObject* rock2;
GLint shaderProgram;
GLint terrainShader;
GLint waterShader;
Terrain * default_ground;
Terrain * lake_ground;
Terrain * coast_ground;
Water * water;
Patch* patch1;
Patch* patch2;
Patch* patch3;
Patch* patch4;
double cursorPosX = 0.0;
double cursorPosY = 0.0;
bool Window::toon = true;
bool Window::illuminate_terr = true;
bool Window::simple_patches = false;

unsigned int ground_type = 0;	// Default ground to render based off of SD heightmap

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"
#define TERR_SHADER_VERT_PATH "../terrainShader.vert"
#define TERR_SHADER_FRAG_PATH "../terrainShader.frag"
#define WATER_SHADER_VERT_PATH "../water.vert"
#define WATER_SHADER_FRAG_PATH "../water.frag"

#define SD_TERRAIN 0
#define LAKE_TERRAIN 1
#define COAST_TERRAIN 2

glm::vec3 patchPts1[16] = { glm::vec3(-9.0f, 0.0f, 9.0f), glm::vec3(-6.0f, 0.0f, 9.0f), glm::vec3(-3.0f, 0.0f, 9.0f), glm::vec3(0.0f, 0.0f, 9.0f),
glm::vec3(-9.0f, 1.0f, 6.0f), glm::vec3(-6.0f, 1.0f, 6.0f), glm::vec3(-3.0f, 0.5f, 6.0f), glm::vec3(0.0f, 0.0f, 6.0f),
glm::vec3(-9.0f, 1.0f, 3.0f), glm::vec3(-6.0f, 1.0f, 3.0f), glm::vec3(-3.0f, 0.5f, 3.0f), glm::vec3(0.0f, 0.0f, 3.0f),
glm::vec3(-9.0f, 0.0f, 0.0f), glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };

glm::vec3 patchPts2[16] = { glm::vec3(0.0f, 0.0f, 9.0f), glm::vec3(3.0f, 0.0f, 9.0f), glm::vec3(6.0f, 0.0f, 9.0f), glm::vec3(9.0f, 0.0f, 9.0f),
glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(3.0f, -0.5f, 6.0f), glm::vec3(6.0f, -1.0f, 6.0f), glm::vec3(9.0f, -1.0f, 6.0f),
glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(3.0f, -0.5f, 3.0f), glm::vec3(6.0f, -1.0f, 3.0f), glm::vec3(9.0f, -1.0f, 3.0f),
glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(9.0f, 0.0f, 0.0f) };

glm::vec3 patchPts3[16] = { glm::vec3(-9.0f, 0.0f, 0.0f), glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
glm::vec3(-9.0f, -1.0f, -3.0f), glm::vec3(-6.0f, -1.0f, -3.0f), glm::vec3(-3.0f, -0.5f, -3.0f), glm::vec3(0.0f, 0.0f, -3.0f),
glm::vec3(-9.0f, -1.0f, -6.0f), glm::vec3(-6.0f, -1.0f, -6.0f), glm::vec3(-3.0f, -0.5f, -6.0f), glm::vec3(0.0f, 0.0f, -6.0f),
glm::vec3(-9.0f, 0.0f, -9.0f), glm::vec3(-6.0f, 0.0f, -9.0f), glm::vec3(-3.0f, 0.0f, -9.0f), glm::vec3(0.0f, 0.0f, -9.0f) };

glm::vec3 patchPts4[16] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.5f), glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(9.0f, 0.0f, 0.0f),
glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(3.0f, 0.5f, -3.0f), glm::vec3(6.0f, 1.0f, -3.0f), glm::vec3(9.0f, 1.0f, -3.0f),
glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(3.0f, 0.5f, -6.0f), glm::vec3(6.0f, 1.0f, -6.0f), glm::vec3(9.0f, 1.0f, -6.0f),
glm::vec3(0.0f, 0.0f, -9.0f), glm::vec3(3.0f, 0.0f, -9.0f), glm::vec3(6.0f, 0.0f, -9.0f), glm::vec3(9.0f, 0.0f, -9.0f) };

// Default camera parameters
glm::vec3 Window::cam_pos(182.0f, 0.0f, -5.0f);		// e  | Position of camera
glm::vec3 Window::cam_look_at(-1.0f, 0.0f, -300.0f);	// d  | This is where the camera looks at
glm::vec3 Window::cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

// To help define the clipping plane
float Window::water_level;
float Window::plane_vec_dir;

void Window::initialize_objects()
{
	skybox = new Cube();
	default_ground = new Terrain();
	lake_ground = new Terrain(1000.0f, 50.0f, -14.0f, "../assets/lake.png", "../assets/textures/grass.ppm");
	coast_ground = new Terrain(1000.0f, 105.0f, -19.0f, "../assets/coast.jpg", "../assets/textures/rocky.ppm");
	water = new Water();
	water->init_FBOs();
	water_level = water->getWaterLevel() + 0.01f;	// Add a small offset for clipping plane to remove glitchy edges

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	terrainShader = LoadShaders(TERR_SHADER_VERT_PATH, TERR_SHADER_FRAG_PATH);
	waterShader = LoadShaders(WATER_SHADER_VERT_PATH, WATER_SHADER_FRAG_PATH);

	anchor = new OBJObject("../assets/object_files/Anchor.obj");
	beachball = new OBJObject("../assets/object_files/beachball.obj");
	chair = new OBJObject("../assets/object_files/beachchair_C.obj");
	crab = new OBJObject("../assets/object_files/Citiezn_snips.obj");
	hut = new OBJObject("../assets/object_files/Hut_obj.obj");
	chair2 = new OBJObject("../assets/object_files/obj.obj");
	rock = new OBJObject("../assets/object_files/Stone_F_3.obj");
	rock2 = new OBJObject("../assets/object_files/Stone_Forest_1.obj");
	patch1 = new Patch(glm::vec3(180.0f, -4.8f, -5.0f), patchPts1);
	patch2 = new Patch(glm::vec3(180.0f, -4.8f, -5.0f), patchPts2);
	patch3 = new Patch(glm::vec3(180.0f, -4.8f, -5.0f), patchPts3);
	patch4 = new Patch(glm::vec3(180.0f, -4.8f, -5.0f), patchPts4);
	// -/+x: Left/Right    -/+y: Down/Up    -/+z: Forward/Back
	anchor->rotate(0.0f, 1.0f, 0.0f, 0.0f);
	anchor->resize(2.0f);
	anchor->move(50.0f, 0.0f, -75.0f);
	beachball->resize(0.5f);
	beachball->move(130.0f, -4.0f, 0.0f);
	chair->rotate(0.0f, 1.0f, 0.0f, -glm::pi<float>() / 8.0f);
	chair->resize(2.0f);
	chair->move(180.0f, -1.8f, -5.0f);
	crab->rotate(0.0f, 1.0f, 0.0f, -glm::pi<float>() / 2.5f);
	crab->resize(0.5f);
	crab->move(200.0f, -4.0f, -10.0f);
	hut->rotate(0.0f, 1.0f, 0.0f, -glm::pi<float>() / 2.0f);
	hut->resize(15.0f);
	hut->move(420.0f, 9.2f, -70.0f);
	chair2->rotate(0.0f, 1.0f, 0.0f, glm::pi<float>() * 1.2f);
	chair2->resize(1.5f);
	chair2->move(135.0f, -1.1f, -155.0f);
	rock->resize(4.0f);
	rock->move(150.0f, 0.0f, 50.0f);
	rock2->resize(2.0f);
	rock2->move(150.0f, 0.0f, -75.0f);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(skybox);
	delete(anchor);
	delete(beachball);
	delete(chair);
	delete(crab);
	delete(hut);
	delete(chair2);
	delete(rock);
	delete(rock2);
	delete(default_ground);
	delete(lake_ground);
	delete(coast_ground);
	delete(water);
	delete(patch1);
	delete(patch2);
	delete(patch3);
	delete(patch4);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(terrainShader);
	glDeleteProgram(waterShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetScrollCallback(window, scroll_callback);
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
}

void Window::display_callback(GLFWwindow* window)
{
	glEnable(GL_CLIP_DISTANCE0);	// Use clipping plane only for reflection/refraction texture creation

	/* Render twice for reflection and refraction*/
	// Reflection texture
	water->resize_FBOs();
	water->bind_reflect_FBO();
	plane_vec_dir = 1.0;
	water_level *= -1.0;
	// position the camera to simulate the reflection texture
	float distance = 2 * (cam_pos.y - water->getWaterLevel());
	float look_at_distance = 2 * (cam_look_at.y - water->getWaterLevel());
	cam_pos.y -= distance;
	cam_look_at.y -= look_at_distance;
	render_scene();
	cam_pos.y += distance;	// Move back to original position
	cam_look_at.y += look_at_distance;

	// Refraction texture
	water->bind_refract_FBO();
	plane_vec_dir = -1.0;
	water_level *= -1.0;
	render_scene();
	water->unbind_FBO();

	glDisable(GL_CLIP_DISTANCE0);

	// Actual scene
	render_scene();

	// Render water
	glUseProgram(waterShader);
	water->draw(waterShader);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::render_scene() {
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);

	// Render
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	skybox->draw(shaderProgram);
	if (ground_type == SD_TERRAIN) {
		anchor->draw(shaderProgram, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.5f, 32.0f), toon);
		beachball->draw(shaderProgram, glm::vec3(0.2f, 0.2f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.7f, 32.0f), toon);
		chair->draw(shaderProgram, glm::vec3(1.0f, 1.0f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.77f, 76.8f), toon);
		crab->draw(shaderProgram, glm::vec3(0.7f, 0.4f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.65f, 76.8f), toon);
		hut->draw(shaderProgram, glm::vec3(0.6f, 0.18f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.2f, 32.0f), toon);
		chair2->draw(shaderProgram, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.7f, 10.0f), toon);
		rock->draw(shaderProgram, glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.2f, 16.0f), toon);
		rock2->draw(shaderProgram, glm::vec3(0.9f, 0.7f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.2f, 16.0f), toon);
		patch1->draw(shaderProgram, glm::vec3(0.0f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.1f, 16.0f), toon, simple_patches);
		patch2->draw(shaderProgram, glm::vec3(0.0f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.1f, 16.0f), toon, simple_patches);
		patch3->draw(shaderProgram, glm::vec3(0.0f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.1f, 16.0f), toon, simple_patches);
		patch4->draw(shaderProgram, glm::vec3(0.0f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.3f, 0.2f, -1.0f), cam_pos, glm::vec4(0.3f, 1.0f, 0.1f, 16.0f), toon, simple_patches);
	}

	glUseProgram(terrainShader);

	// Draw different types of terrain
	switch (ground_type) {
	case 0:
		default_ground->draw(terrainShader);
		break;
	case 1:
		lake_ground->draw(terrainShader);
		break;
	case 2:
		coast_ground->draw(terrainShader);
		break;
	}
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			//Move camera left
			glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
			cam_dir = glm::normalize(glm::cross(cam_dir, cam_up));
			cam_pos = cam_pos - cam_dir;
			cam_look_at = cam_look_at - cam_dir;
		}
		else if (key == GLFW_KEY_D)
		{
			//Move camera right
			glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
			cam_dir = glm::normalize(glm::cross(cam_dir, cam_up));
			cam_pos = cam_pos + cam_dir;
			cam_look_at = cam_look_at + cam_dir;
		}
		else if (key == GLFW_KEY_W)
		{
			//Move camera forwards
			glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
			cam_pos = cam_pos + cam_dir;
			cam_look_at = cam_look_at + cam_dir;
		}
		else if (key == GLFW_KEY_S)
		{
			//Move camera backwards
			glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
			cam_pos = cam_pos - cam_dir;
			cam_look_at = cam_look_at - cam_dir;
		}
		else if (key == GLFW_KEY_Z)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				//Move out of the screen (closer)
			}
			else
			{
				//Move into the screen (farther)
			}
		}
		else if (key == GLFW_KEY_C)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				//Scale up
			}
			else
			{
				//Scale down
			}
		}
		else if (key == GLFW_KEY_R)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				//Reset rotation and scale
			}
			else
			{
				//Reset camera zoom
				glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
				cam_dir = glm::vec3(cam_dir.x * 20, cam_dir.y * 20, cam_dir.z * 20);
				cam_pos = cam_look_at - cam_dir;
			}
		}
		else if (key == GLFW_KEY_1)
		{
			//Toggle toon shading
			toon = !toon;
		}
		else if (key == GLFW_KEY_2)
		{
			//Toggle simple surface patches
			simple_patches = !simple_patches;
			patch1->reinitialize(simple_patches);
			patch2->reinitialize(simple_patches);
			patch3->reinitialize(simple_patches);
			patch4->reinitialize(simple_patches);
		}
		else if (key == GLFW_KEY_T) {
			if (mods == GLFW_MOD_SHIFT)
			{
				ground_type = (ground_type + 1) % 3;	// Toggle between different grounds
			}
			else
			{
				illuminate_terr = !illuminate_terr;
			}
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Moving the mouse with left click rotates the camera in place
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::vec3 prev = trackBallMapping(glm::vec3(cursorPosX, cursorPosY, 0.0f));
		glm::vec3 next = trackBallMapping(glm::vec3(xpos, ypos, 0.0f));
		glm::vec3 dir = next - prev;
		float vel = dir.length();
		if (vel > 0.0001f)
		{
			glm::vec3 rotAxis = glm::cross(prev, next);
			float rotAngle = vel * 0.02f;
			glm::vec4 camDir = glm::vec4(cam_look_at - cam_pos, 1.0f);
			camDir = glm::rotate(glm::mat4(1.0f), -rotAngle, rotAxis) * camDir;
			cam_look_at = glm::vec3(cam_pos.x + camDir.x, cam_pos.y + camDir.y, cam_pos.z + camDir.z);
		}
	}
	//Moving the mouse with right click moves the camera up and down with respect to the world
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		//float xDist = (xpos - cursorPosX) * 0.2f;
		float xDist = 0;
		float yDist = (ypos - cursorPosY) * 0.2f;
		//Vertical movement vector
		glm::vec3 cam_move_v = glm::vec3(cam_up.x * yDist, cam_up.y * yDist, cam_up.z * yDist);
		//Horizontal movement vector (cross camera up vector with the direction the camera is pointing)
		glm::vec3 cam_move_h = glm::normalize(glm::cross(cam_up, cam_look_at - cam_pos));
		cam_move_h = glm::vec3(cam_move_h.x * xDist, cam_move_h.y * xDist, cam_move_h.z * xDist);
		cam_pos = cam_pos - cam_move_v - cam_move_h;
		cam_look_at = cam_look_at - cam_move_v - cam_move_h;
	}
	cursorPosX = xpos;
	cursorPosY = ypos;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Scrolling zooms in and out
	glm::vec3 cam_dir = glm::normalize(cam_look_at - cam_pos);
	cam_dir = glm::vec3(cam_dir.x * yoffset, cam_dir.y * yoffset, cam_dir.z * yoffset);
	if (yoffset < 0 || glm::distance(cam_look_at, cam_pos) > glm::length(cam_dir))
	{
		cam_pos += cam_dir;
	}
}

glm::vec3 Window::trackBallMapping(glm::vec3 point)
{
	glm::vec3 v = glm::vec3((2.0f * point.x - width) / width, (height - 2.0  * point.y) / height, 0.0);
	float d = glm::length(v);
	d = d < 1.0 ? d : 1.0;
	v.z = sqrtf(1.001 - d*d);
	return glm::normalize(v);
}
