/* 
 * By Ronald Allan V. Baldonado
 * Vertex shader used for normal shading (doesn't shade anything here)
 * Mostly used for testing purposes
 */
#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 view;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
out vec3 normal_vec;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	normal_vec = mat3(transpose(inverse(model))) * normal;;
}
