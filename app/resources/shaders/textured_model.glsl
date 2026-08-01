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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emissive1;
uniform vec3 globalAmbient; //One color for the whole scene
uniform bool useEmissive;

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 result = (color * globalAmbient);
    if (useEmissive) {
        vec3 emissive = texture(texture_emissive1, TexCoords).rgb;
        vec3 customGlowColor = vec3(3.0, 0.2, 1.5); // Vrednosti preko 1.0 daju "HDR / Bloom" efekat jačine
        result += emissive * customGlowColor;
    }
    FragColor = vec4(result, 1.0);
}