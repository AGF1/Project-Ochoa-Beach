#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 pLightPos;
uniform vec3 sLightPos;
uniform float sLightExp;
uniform int mode;
uniform mat4 modelview;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 FragPos;
out vec3 Normal;
out vec3 PLightDir;
out vec3 SLightDir;
out vec3 Eye;
out vec3 TexCoords;
out float PAttenuate;
out float SAttenuate;
out float SLightExp;

void main()
{
    // Point/Spot light code
    vec4 pos = modelview * vec4(position.x, position.y, position.z, 1.0);
    PLightDir = vec3(pLightPos - pos);
	SLightDir = vec3(sLightPos - pos);
    Eye = vec3(-pos);
	//float pDistance = sqrt(pow(pLightPos.x - pos.x, 2) + pow(pLightPos.y - pos.y, 2) + pow(pLightPos.z - pos.z, 2));
	//float sDistance = sqrt(pow(sLightPos.x - pos.x, 2) + pow(sLightPos.y - pos.y, 2) + pow(sLightPos.z - pos.z, 2));
	PAttenuate = 1.0 / (0.5 * length(pLightPos - pos));
	SAttenuate = 1.0 / (0.1 * pow(length(sLightPos - pos), 2.0));

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M

	if (mode == 2)
	{
		gl_Position = projection * view * vec4(position.x, position.y, position.z, 1.0);
	}
	else
	{
		gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	}
    FragPos = vec3(model * vec4(position.x, position.y, position.z, 1.0));
    Normal = mat3(transpose(inverse(modelview))) * normal;
	//vec4 edit = view * model * vec4(normal.x, normal.y, normal.z, 1.0);
    //Normal = vec3(edit.x, edit.y, edit.z);
	SLightExp = sLightExp;
	TexCoords = position;
}
