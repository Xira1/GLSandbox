#version 460
 
uniform samplerCube environmentMap;
uniform float darknessFactor;
in vec3 position;
out vec4 fragOut;

void main () {
    vec4 color = texture(environmentMap, position);
    fragOut = vec4(color.rgb * darknessFactor, color.a);
}