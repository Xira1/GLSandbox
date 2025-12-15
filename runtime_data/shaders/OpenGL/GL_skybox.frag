#version 460 core
 
layout (location = 0) out vec4 FinalLightingOut ;
layout (location = 1) out vec4 WorldPositionOut;

layout (binding = 0) uniform samplerCube skybox;

in vec3 TexCoords;
in vec4 WorldPos;

uniform float darknessFactor;

void main() { 
    vec3 skyColor = texture(skybox, TexCoords).rgb;
    vec3 skyLinear = pow(skyColor, vec3(2.2));

    FinalLightingOut = vec4(skyLinear, 1.0);
    WorldPositionOut = vec4(WorldPos.rgb * vec3(10, 1, 10), 1.0);
}