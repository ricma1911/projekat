//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emissive1;
uniform samplerCube depthMap0;
uniform samplerCube depthMap1;
uniform float far_plane;
uniform vec3 globalAmbient;
uniform bool useEmissive;
uniform vec3 emissiveColor;

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 2
uniform SpotLight spotLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

float PointShadowCalculation(samplerCube depthMapTex, vec3 fragPos, vec3 lightPos);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos);

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);

    vec3 result = color * globalAmbient;

    vec3 spotContrib = color * CalcSpotLight(spotLight, norm, FragPos);

    vec3 lightDir = normalize(spotLight.position - FragPos);
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float spotMask = clamp((theta - spotLight.outerCutOff) / (spotLight.cutOff - spotLight.outerCutOff), 0.0, 1.0);

    vec3 pointContrib = vec3(0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        float shadow = 0.0;
        if (i == 0) {
            shadow = PointShadowCalculation(depthMap0, FragPos, pointLights[0].position);
        } else if (i == 1) {
            shadow = PointShadowCalculation(depthMap1, FragPos, pointLights[1].position);
        }

        vec3 lightColor = color * CalcPointLight(pointLights[i], norm, FragPos);
        pointContrib += lightColor * (1.0 - shadow);
    }
    result += pointContrib * (1.0 - spotMask);

    result += spotContrib;

    if (useEmissive) {
        vec3 emissive = texture(texture_emissive1, TexCoords).rgb;
        result += emissive * emissiveColor;

        if (length(emissive) > 0.1) {
            BrightColor = vec4(emissive * emissiveColor * 2.0, 1.0);
        } else {
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    FragColor = vec4(result, 1.0);
}

float PointShadowCalculation(samplerCube depthMapTex, vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float closestDepth = texture(depthMapTex, fragToLight).r;
    closestDepth *= far_plane;

    float bias = 0.15;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return light.color * diff * intensity * attenuation;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return light.color * diff * attenuation;
}