#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 view;
uniform vec3 cam_pos;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
out vec2 texPos;
out vec4 clipSpace;
out vec3 eyeVec;

// Constants
const float tile = 9.0;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    //gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	//clipSpace = gl_Position;

	vec4 worldPos = model * vec4(position, 1.0);
	clipSpace = projection * modelview * vec4(position, 1.0);
	gl_Position = clipSpace;

	//texPos = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * tile;
	texPos = tex_coord * tile;

	eyeVec = cam_pos - vec3(worldPos);
}