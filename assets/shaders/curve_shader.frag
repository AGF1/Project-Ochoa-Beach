/* 
 * By Ronald Allan V. Baldonado
 * Fragment shader for the uniform-sampled Bezier curve
 */

#version 330 core
// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
    // Normal coloring
	color = vec4(0.0, 0.0, 0.0, 1.0);
}
