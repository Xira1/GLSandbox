#version 460 core

layout (location = 0) out vec4 LightingOut;

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
    vec3 albedo = texture2D(baseColorTexture, TexCoord).rgb;
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;

    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
    float roughness = rma.r;
    float metallic = rma.g;
    float ao = rma.b;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - WorldPos);
    vec3 viewDir = normalize(viewPos - WorldPos);

    // ambient and AO
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * ao;

    // diffuse for both side
    float diff = max(dot(norm, lightDir), 0.0) + max(dot(-norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular and roughness
    float specularStrength = mix(1.0, 0.1, roughness);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * lightColor;

    // attenuation
    float distance = length(lightPos - WorldPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance)); // if >, than ligth is dimmer 0.09 and 0.032 is best for my scene

    vec3 lightning = (ambient + diffuse + specular) * attenuation * lightIntensity;
    vec3 result = lightning * albedo;

    LightingOut = vec4(result, 1.0);
}


