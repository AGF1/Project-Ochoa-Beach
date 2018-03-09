#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 FragPos;
in vec3 Normal;
in vec3 PLightDir;
in vec3 SLightDir;
in vec3 Eye;
in vec3 TexCoords;
in float PAttenuate;
in float SAttenuate;
in float SLightExp;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 pLightColor;
uniform vec3 sLightColor;
uniform vec3 sLightAim;
uniform float sLightCut;
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
	    vec3 ambient = ambientModifier * vec3(1.0f, 1.0f, 1.0f);
	    vec3 norm = normalize(Normal);
		vec3 pLightNorm = normalize(PLightDir);
		vec3 sLightNorm = normalize(SLightDir);
		vec3 sLightAimNorm = normalize(sLightAim);
		vec3 eyeNorm = normalize(Eye);

	    float diff = max(dot(norm, lightDir), 0.0);
		float pDiff = max(dot(norm, pLightNorm), 0.0);
		float sDiff = max(dot(norm, sLightNorm), 0.0);

	    vec3 diffuse = diffuseModifier * diff * lightColor;
		vec3 pDiffuse = PAttenuate * diffuseModifier * pDiff * pLightColor;
		vec3 sDiffuse = SAttenuate * diffuseModifier * sDiff * sLightColor;

	    vec3 viewDir = normalize(camPos - FragPos);
	    vec3 reflectDir = reflect(-lightDir, norm);
		vec3 fromPoint = normalize(pLightNorm + eyeNorm);
		vec3 fromSpot = normalize(sLightNorm + eyeNorm);

	    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		float pSpec = pow(max(dot(fromPoint, norm), 0.0), shininess);
		float sSpec = pow(max(dot(fromSpot, norm), 0.0), shininess);

	    vec3 specular = specularModifier * spec * lightColor;
		vec3 pSpecular = PAttenuate * specularModifier * pSpec * pLightColor;
		vec3 sSpecular = SAttenuate * specularModifier * sSpec * sLightColor;
		vec3 result = vec3(0.0, 0.0, 0.0);
		float loc = dot(-sLightNorm, sLightAimNorm);
		if (loc > sLightCut)
		{
			result = (ambient + diffuse + pDiffuse + sDiffuse * pow(loc, SLightExp) + specular + pSpecular + sSpecular * pow(loc, SLightExp)) * objectColor;
		}
		else
		{
			result = (ambient + diffuse + pDiffuse + specular + pSpecular) * objectColor;
		}

        color = vec4(result, 1.0);
	}
	else if (mode == 1)
	{
	    vec3 norm = normalize(Normal) * 0.5 + 0.5;
		color = vec4(norm, 1.0);
	}
	else if (mode == 2)
	{
		color = texture(skybox, TexCoords);
	}
	else if (mode == 3)
	{
		color = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
