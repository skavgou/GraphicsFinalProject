#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV; // Handle UVs

// Output data, to be interpolated for each fragment
out vec3 color;
out vec2 uv;                // Pass UV to fragment shader
out vec4 fragPosLightSpace; // Pass light-space position to fragment shader

// Matrices for transformations
uniform mat4 MVP;           // Model-View-Projection matrix
uniform mat4 LMAP;          // Light-space matrix

void main() {
    // Transform vertex for main rendering
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass color and UV to the fragment shader
    color = vertexColor;
    uv = vertexUV;

    // Calculate and pass the light-space position
    fragPosLightSpace = LMAP * vec4(vertexPosition, 1.0);
}
