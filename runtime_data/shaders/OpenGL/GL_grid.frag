#version 460 core

layout (location = 0) out vec4 FragColor;

uniform vec3 uCameraPosition;
uniform vec3 uGridColor;
uniform float uGridSize;       
uniform float uMajorFactor;    
uniform mat4 uInverseViewProjection;
uniform vec2 uResolution;

vec3 WorldPos(vec2 uv, mat4 invViewProj) {
    vec4 clip = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
    vec4 world = invViewProj * clip;
    return world.xyz / world.w;
}

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;
    vec3 worldPos = WorldPos(uv, uInverseViewProjection);

    vec3 rayDir = normalize(worldPos - uCameraPosition);
    float t = -uCameraPosition.y / rayDir.y;
    vec3 groundPos = uCameraPosition + rayDir * t;

    if (t < 0.0) discard;
    
    // SMALL
    vec2 baseCoord = groundPos.xz  / uGridSize;
    vec2 gridSmall = abs(fract(baseCoord - 0.5) - 0.5);
    vec2 derivSmall = fwidth(baseCoord);
    float lineXSmall = smoothstep(0.008 - derivSmall.x, 0.008 + derivSmall.x, gridSmall.x);
    float lineYSmall = smoothstep(0.008 - derivSmall.y, 0.008 + derivSmall.y, gridSmall.y);
    float alphaSmall = 1.0 - min(lineXSmall, lineYSmall);

    // MAJOR
    vec2 majorCoord = baseCoord / uMajorFactor;
    vec2 gridLarge = abs(fract(majorCoord - 0.5) - 0.5);
    vec2 derivLarge = derivSmall / uMajorFactor;
    float lineXLarge = smoothstep(0.006 - derivLarge.x, 0.006 + derivLarge.x, gridLarge.x);
    float lineYLarge = smoothstep(0.006 - derivLarge.y, 0.006 + derivLarge.y, gridLarge.y);
    float alphaLarge = 1.0 - min(lineXLarge, lineYLarge);

    float distance = length(groundPos - uCameraPosition);
    float fadeStart = 30.0;
    float fadeEnd = 120.0;
    float fade = smoothstep(fadeEnd, fadeStart, distance);

    alphaSmall *= fade;
    alphaLarge *= fade;

    vec4 colorSmall = vec4(uGridColor * 0.6, alphaSmall);
    vec4 colorLarge = vec4(uGridColor, alphaLarge);      

    float finalAlpha = max(colorSmall.a, colorLarge.a);
    if (finalAlpha <= 0.0) discard;

    vec3 finalColor = mix(colorSmall.rgb, colorLarge.rgb, step(colorSmall.a, colorLarge.a));

    FragColor = vec4(finalColor, finalAlpha);
}