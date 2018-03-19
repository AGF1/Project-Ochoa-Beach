#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "OBJObject.h"
#include "Node.h"
#include "Transform.h"
#include "Geometry.h"
#include "shader.h"
#include "Curve.h"
#include "Terrain.h"
#include "Water.h"

class Window
{
public:
	static int width;
	static int height;
	static float water_level;
	static float plane_vec_dir;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static glm::vec3 cam_pos;
	static glm::vec3 cam_look_at;
	static glm::vec3 cam_up;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackBallMapping(glm::vec3 point);

private:
	static void render_scene(); // Object rendering minus water goes here
};

#endif
