#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
in vec3 texPos;

// Uniform variables
uniform sampler2D terrain;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main() { 
	//color = texture(terrain, texPos); 
	color = vec4(0.0, 0.0, 0.0, 1.0);
}