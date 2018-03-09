#include "window.h"

const char* window_title = "GLFW Starter Project";
Cube * skybox;
GLint shaderProgram;
double cursorPosX = 0.0;
double cursorPosY = 0.0;
Node* scenegraph;
Curve* c;
GLfloat points[2250];
int timer = 0;

glm::vec3 curve1[4] = {
	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 4.0f), glm::vec3(12.0f, 0.0f, 4.0f), glm::vec3(16.0f, 0.0f, 0.0f)
};

glm::vec3 curve2[4] = {
	glm::vec3(16.0f, 0.0f, 0.0f), glm::vec3(20.0f, 0.0f, -4.0f), glm::vec3(28.0f, 0.0f, -8.0f), glm::vec3(32.0f, 0.0f, -4.0f)
};

glm::vec3 curve3[4] = {
	glm::vec3(32.0f, 0.0f, -4.0f), glm::vec3(36.0f, 0.0f, 0.0f), glm::vec3(44.0f, 0.0f, -24.0f), glm::vec3(36.0f, 0.0f, -20.0f)
};

glm::vec3 curve4[4] = {
	glm::vec3(36.0f, 0.0f, -20.0f),glm::vec3(28.0f, 0.0f, -16.0f),glm::vec3(24.0f, 0.0f, -20.0f),glm::vec3(20.0f, 0.0f, -24.0f)
};

glm::vec3 curve5[4] = {
	glm::vec3(20.0f, 0.0f, -24.0f), glm::vec3(16.0f, 0.0f, -28.0f), glm::vec3(-4.0f, 0.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f)
};

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 Window::cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 Window::cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 Window::cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	skybox = new Cube();

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	scenegraph = assembleMyRobotArmyOfEvil();
	genPoints();
	c = new Curve(points);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(skybox);
	delete(scenegraph);
	delete(c);
	glDeleteProgram(shaderProgram);
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
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function
	scenegraph->update();
	timer++;
	if (timer >= 750) timer = 0;
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	skybox->draw(shaderProgram);
	glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(points[timer * 3], points[timer * 3 + 1], points[timer * 3 + 2]));
	scenegraph->draw(V * M);
	c->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
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

Node* Window::assembleMyRobotArmyOfEvil()
{
	Transform* root = new Transform(glm::mat4(1.0f), false);

	Geometry* body = new Geometry(shaderProgram);
	body->init("body.obj");
	Geometry* head = new Geometry(shaderProgram);
	head->init("head.obj");
	Geometry* limb = new Geometry(shaderProgram);
	limb->init("limb.obj");
	Geometry* eyeball = new Geometry(shaderProgram);
	eyeball->init("eyeball.obj");
	Geometry* antenna = new Geometry(shaderProgram);
	antenna->init("antenna.obj");

	Transform* bodyTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>() / -2, glm::vec3(1.0f, 0.0f, 0.0f)), false);
	bodyTransform->addChild(body);

	Transform* headTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)), false);
	headTransform->addChild(head);
	bodyTransform->addChild(headTransform);

	Transform* rightArmTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 25.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-26.75f, 0.0f, 45.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(26.75f, 0.0f, -45.0f)), true);
	rightArmTransform->addChild(limb);
	bodyTransform->addChild(rightArmTransform);

	Transform* leftArmTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(52.25f, 0.0f, 25.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-26.75f, 0.0f, 45.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(26.75f, 0.0f, -45.0f)), true);
	leftArmTransform->addChild(limb);
	bodyTransform->addChild(leftArmTransform);

	Transform* rightLegTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, -15.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-26.75f, 0.0f, 45.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(26.75f, 0.0f, -45.0f)), true);
	rightLegTransform->addChild(limb);
	bodyTransform->addChild(rightLegTransform);

	Transform* leftLegTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(37.25f, 0.0f, -15.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-26.75f, 0.0f, 45.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(26.75f, 0.0f, -45.0f)), true);
	leftLegTransform->addChild(limb);
	bodyTransform->addChild(leftLegTransform);

	Transform* rightEyeballTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-7.5f, -4.0f, 87.5f)), false);
	rightEyeballTransform->addChild(eyeball);
	headTransform->addChild(rightEyeballTransform);

	Transform* leftEyeballTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(7.5f, -4.0f, 87.5f)), false);
	leftEyeballTransform->addChild(eyeball);
	headTransform->addChild(leftEyeballTransform);

	Transform* rightAntennaTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-7.5f, 0.0f, 87.5f)), false);
	rightAntennaTransform->addChild(antenna);
	headTransform->addChild(rightAntennaTransform);

	Transform* leftAntennaTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(7.5f, 0.0f, 87.5f)) * glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)), false);
	leftAntennaTransform->addChild(antenna);
	headTransform->addChild(leftAntennaTransform);

	//Instancing
	for (float i = -5.0f; i <= 5.0f; i += 2.5f)
	{
		for (float j = -5.0f; j <= 5.0f; j += 2.5f)
		{
			Transform* robotTransform = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, j)), false);
			robotTransform->addChild(bodyTransform);
			root->addChild(robotTransform);
		}
	}

	return root;
}

void Window::genPoints()
{
	for (int i = 0; i < 150; i++)
	{
		float t = i / 150.0f;
		glm::vec3 x = (glm::pow(1 - t, 3)) * curve1[0]
			+ (3 * glm::pow(1 - t, 2) * t) * curve1[1]
			+ (3 * (1 - t) * glm::pow(t, 2)) * curve1[2]
			+ (glm::pow(t, 3)) * curve1[3];
		points[3 * i] = x.x;
		points[3 * i + 1] = x.y;
		points[3 * i + 2] = x.z;
	}
	for (int i = 0; i < 150; i++)
	{
		float t = i / 150.0f;
		glm::vec3 x = (glm::pow(1 - t, 3)) * curve2[0]
			+ (3 * glm::pow(1 - t, 2) * t) * curve2[1]
			+ (3 * (1 - t) * glm::pow(t, 2)) * curve2[2]
			+ (glm::pow(t, 3)) * curve2[3];
		points[3 * i + 450] = x.x;
		points[3 * i + 451] = x.y;
		points[3 * i + 452] = x.z;
	}
	for (int i = 0; i < 150; i++)
	{
		float t = i / 150.0f;
		glm::vec3 x = (glm::pow(1 - t, 3)) * curve3[0]
			+ (3 * glm::pow(1 - t, 2) * t) * curve3[1]
			+ (3 * (1 - t) * glm::pow(t, 2)) * curve3[2]
			+ (glm::pow(t, 3)) * curve3[3];
		points[3 * i + 900] = x.x;
		points[3 * i + 901] = x.y;
		points[3 * i + 902] = x.z;
	}
	for (int i = 0; i < 150; i++)
	{
		float t = i / 150.0f;
		glm::vec3 x = (glm::pow(1 - t, 3)) * curve4[0]
			+ (3 * glm::pow(1 - t, 2) * t) * curve4[1]
			+ (3 * (1 - t) * glm::pow(t, 2)) * curve4[2]
			+ (glm::pow(t, 3)) * curve4[3];
		points[3 * i + 1350] = x.x;
		points[3 * i + 1351] = x.y;
		points[3 * i + 1352] = x.z;
	}
	for (int i = 0; i < 150; i++)
	{
		float t = i / 150.0f;
		glm::vec3 x = (glm::pow(1 - t, 3)) * curve5[0]
			+ (3 * glm::pow(1 - t, 2) * t) * curve5[1]
			+ (3 * (1 - t) * glm::pow(t, 2)) * curve5[2]
			+ (glm::pow(t, 3)) * curve5[3];
		points[3 * i + 1800] = x.x;
		points[3 * i + 1801] = x.y;
		points[3 * i + 1802] = x.z;
	}
}
