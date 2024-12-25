#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;

// Uniforms
uniform mat4 model;           // Model matrix for the object
uniform mat4 lightSpaceMatrix; // Light-space transformation matrix

// Output to the fragment shader
out vec4 fragPosLightSpace;

void main() {
    // Transform vertex position into light-space coordinates
    fragPosLightSpace = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);

    // Transform into clip space
    gl_Position = fragPosLightSpace;
}
