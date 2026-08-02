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
    Normal = aNormal;
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

uniform SpotLight spotLight;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    if (color.r > 0.99f && color.g > 0.99f && color.b > 0.99f) {
        if (int(gl_FragCoord.x + gl_FragCoord.y) % 2 == 0)
        discard;
    }

    vec3 norm = normalize(Normal);
    vec3 result = color * globalAmbient;
    result += color * CalcSpotLight(spotLight, norm, FragPos);

    FragColor = vec4(result, 1.0);
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