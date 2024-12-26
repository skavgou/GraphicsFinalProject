#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

uniform mat4 MVP;
uniform mat4 lightSpaceMatrix;

out vec4 fragPosLightSpace;
out vec2 TexCoords;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    fragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
    TexCoords = uv;
}
