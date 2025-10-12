#version 460
 
layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float darknessFactor;

void main() { 
    vec4 color = texture(skybox, TexCoords);
    FragColor = vec4(color.rgb * darknessFactor, color.a);
}