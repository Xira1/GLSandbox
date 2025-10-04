#version 460 core

layout (location = 0) out vec4 FragColor;

in vec3 Color;

uniform vec4 uniformColor;

void main() {
    FragColor.rgb = Color;
    FragColor.a = 1.0;

    FragColor = uniformColor;    
}