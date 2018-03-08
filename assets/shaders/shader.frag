/* 
 * By Ronald Allan V. Baldonado
 * Fragment shader for normal coloring
 * Mostly used for testing purposes
 */
#version 330 core
// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normal_vec;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
    // Normal coloring
	vec3 normalized = normalize(normal_vec);
	color = vec4(normalized, 1.0);
}
