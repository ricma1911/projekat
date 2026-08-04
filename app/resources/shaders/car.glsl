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

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 globalAmbient;
uniform vec3 viewPos;

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

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo);

void main() {
vec3 rawColor = texture(texture_diffuse1, TexCoords).rgb;
vec3 albedo = mix(rawColor, vec3(0.8), 0.15);

vec3 norm = normalize(Normal);
vec3 viewDir = normalize(viewPos - FragPos);

vec3 result = albedo * globalAmbient;

vec3 spotContrib = CalcSpotLight(spotLight, norm, FragPos, viewDir, albedo);

vec3 lightDir = normalize(spotLight.position - FragPos);
float theta = dot(lightDir, normalize(-spotLight.direction));
float spotMask = clamp((theta - spotLight.outerCutOff) / (spotLight.cutOff - spotLight.outerCutOff), 0.0, 1.0);

vec3 pointContrib = vec3(0.0);
for(int i = 0; i < NR_POINT_LIGHTS; i++) {
pointContrib += CalcPointLight(pointLights[i], norm, FragPos, viewDir, albedo);
}
result += pointContrib * (1.0 - spotMask);

result += spotContrib;

FragColor = vec4(result, 1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
vec3 lightDir = normalize(light.position - fragPos);

float theta = dot(lightDir, normalize(-light.direction));
float epsilon = light.cutOff - light.outerCutOff;
float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

float diff = max(dot(normal, lightDir), 0.0);

vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

float distance = length(light.position - fragPos);
float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

vec3 diffuse = light.color * diff * albedo;
vec3 specular = light.color * spec * 0.5f;

return (diffuse + specular) * intensity * attenuation;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo) {
vec3 lightDir = normalize(light.position - fragPos);

float diff = max(dot(normal, lightDir), 0.0);

vec3 reflectDir = reflect(-lightDir, normal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

float distance = length(light.position - fragPos);
float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

vec3 diffuse = light.color * diff * albedo;
vec3 specular = light.color * spec * 0.5f;

return (diffuse + specular) * attenuation;
}