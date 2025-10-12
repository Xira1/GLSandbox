#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexture;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 WorldPos;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {

	vec4 worldPos = model * vec4(aPos, 1.0);
	WorldPos = worldPos.xyz;
	
	TexCoord = aTexture;

	mat3 normalMat = transpose(inverse(mat3(model)));
	Normal = normalize(normalMat * aNormal);
	Tangent = normalize(normalMat * aTangent);
	Bitangent = normalize(normalMat * aBitangent);

	gl_Position = projection * view * worldPos;
}