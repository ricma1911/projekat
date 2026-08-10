//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 shadowMatrix;
uniform mat4 model;

out vec3 FragPos;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = shadowMatrix * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

in vec3 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    float lightDistance = length(FragPos - lightPos);
    lightDistance = lightDistance / far_plane;
    gl_FragDepth = lightDistance;
}