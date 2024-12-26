#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV; // Texture coordinates

// Output data, to be interpolated for each fragment
out vec3 color;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 UV; // Texture coordinates

uniform mat4 MVP; // Model-View-Projection matrix
uniform mat4 modelMatrix;   // Model matrix

void main() {
    // Transform vertex position
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass attributes to the fragment shader
    color = vertexColor;
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;
    UV = vertexUV; // Pass UV coordinates
}
