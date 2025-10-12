#version 460 core

layout (location = 0) out vec4 LightingOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;

layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 WorldPos;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

void main() {
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    baseColor.rgb = pow(baseColor.rgb, vec3(2.2));

    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb * 2.0 - 1.0;
    mat3 TBN = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));
    vec3 worldNormal = normalize(TBN * normalMap);

    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
    float roughness = rma.r;
    float metallic = rma.g;
    float ao = rma.b;

    vec3 L = normalize(lightPos - WorldPos);
    vec3 V = normalize(viewPos - WorldPos);
    vec3 H = normalize(L + V);

    float diff = max(dot(worldNormal, L), 0.0);
    float spec = pow(max(dot(worldNormal, H), 0.0), mix(8.0, 64.0, 1.0 - roughness));

    vec3 diffuse = baseColor.rgb * diff * lightColor * lightIntensity;
    vec3 specular = lightColor * spec * 0.25;

    vec3 ambient = baseColor.rgb * 0.05 * ao;

    vec3 color = ambient + diffuse + specular;

    color = pow(color, vec3(1.0 / 3.8));

    LightingOut = vec4(color, baseColor.a);
    NormalOut = vec4(worldNormal, 1.0);
    RMAOut = vec4(rma, 1.0);
}