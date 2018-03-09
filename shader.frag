#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 FragPos;
in vec3 Normal;
in vec3 Eye;
in vec3 TexCoords;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 camPos;
uniform float ambientModifier;
uniform float diffuseModifier;
uniform float specularModifier;
uniform float shininess;
uniform int mode;
uniform samplerCube skybox;

void main()
{
    if (mode == 0)
	{
		//Phong Shading Code
	    vec3 ambient = ambientModifier * vec3(1.0f, 1.0f, 1.0f);
	    vec3 norm = normalize(Normal);
		vec3 eyeNorm = normalize(Eye);

	    float diff = max(dot(norm, lightDir), 0.0);

	    vec3 diffuse = diffuseModifier * diff * lightColor;

	    vec3 viewDir = normalize(camPos - FragPos);
	    vec3 reflectDir = reflect(-lightDir, norm);

	    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	    vec3 specular = specularModifier * spec * lightColor;
		vec3 result = vec3(0.0, 0.0, 0.0);

		result = (ambient + diffuse + specular) * objectColor;

        color = vec4(result, 1.0);
	}
	else if (mode == 1)
	{
		//Normal Shading Code
	    vec3 norm = normalize(Normal) * 0.5 + 0.5;
		color = vec4(norm, 1.0);
	}
	else if (mode == 2)
	{
		//Skybox Shading Code
		color = texture(skybox, TexCoords);
	}
	else if (mode == 3)
	{
		//Black Shading Code (for the curves)
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
