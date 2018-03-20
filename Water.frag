#version 330 core
// Water shader

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
in vec2 texPos;
in vec4 clipSpace;
in vec3 eyeVec;

// Uniform variables
uniform sampler2D reflect_texture;
uniform sampler2D refract_texture;
uniform sampler2D dudv_map;
uniform sampler2D normal_map;
uniform samplerCube skybox;
uniform vec3 light_color;
uniform vec3 light_dir;
uniform float move_factor;			// For creating water ripples

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

// Constants
const float wave_strength = 0.06;
const float shininess = 100.0;
const float reflectiveness = 0.8;

void main() { 
	vec3 viewVec = normalize(eyeVec);
	vec2 norm_dev_coords = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = vec2(norm_dev_coords.x, norm_dev_coords.y);
	vec2 reflectTexCoords = vec2(norm_dev_coords.x, -norm_dev_coords.y);

	// Distort reflect and refract texture coordinates
	vec2 distortTexCoords = texture(dudv_map, vec2(texPos.x + move_factor, texPos.y)).rg * 0.1;
	distortTexCoords = texPos + vec2(distortTexCoords.x, distortTexCoords.y + move_factor);
	vec2 total_distort = (texture(dudv_map, distortTexCoords).rg * 2.0 - 1.0) * wave_strength;

	reflectTexCoords += total_distort;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);	// Don't let it go past 0 or 1
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);	// Flip to negative due to reflection nature
	refractTexCoords += total_distort;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	
	vec4 reflectColor = texture(reflect_texture, reflectTexCoords);
	vec4 refractColor = texture(refract_texture, refractTexCoords);

	float refractFactor = dot(viewVec, vec3(0.0, 1.0, 0.0));	// Fresnel effect
	refractFactor = pow(refractFactor, 0.4);					// 0-1 = more refractive, 1+ = more reflective

	// Add normal map for specular lighting
	vec4 normalMapColor = texture(normal_map, distortTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	// Specular for water highlights
	vec3 reflectDir = reflect(-light_dir, normal);
	float spec = pow(max(dot(viewVec, reflectDir), 0.0), shininess);
	vec3 specular = light_color * spec * reflectiveness;

	// Calculate skybox reflection
	vec3 skyboxReflectTex = reflect(-viewVec, vec3(0.0, 1.0, 0.0));
	skyboxReflectTex.x += total_distort.x;
	skyboxReflectTex.y += total_distort.y;	// Flip to negative due to reflection nature
	vec4 skyReflectColor = vec4(texture(skybox, skyboxReflectTex).rgb, 1.0);
	
	// Mix the resulting textures together
	color = mix(reflectColor, refractColor, refractFactor);
	skyReflectColor = mix(skyReflectColor, refractColor, refractFactor);
	color = mix(reflectColor, skyReflectColor, 0.72);
	color = mix(color, vec4(0.0, 0.35, 0.75, 1.0), 0.2) + vec4(specular, 0.0);	// Mix with blue tint and add specular lighting
	//color = skyReflectColor;
}