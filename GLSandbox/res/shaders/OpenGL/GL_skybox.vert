#version 460 core
#include "../common/types.glsl"

layout (location = 0) in vec3 inPosition;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
    ViewportData viewportData[];
};

out vec3 TexCoords;
out vec4 WorldPos;

uniform mat4 u_modelMatrix;

void main() {
    TexCoords = inPosition;
    int viewportIdx = gl_BaseInstance;
    mat4 projectionView = viewportData[viewportIdx].skyboxProjectionView;
    vec3 viewPos = viewportData[viewportIdx].inverseView[3].xyz;

    WorldPos = u_modelMatrix * vec4(inPosition, 1.0);
    gl_Position = projectionView * WorldPos;
}  