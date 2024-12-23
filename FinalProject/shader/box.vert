#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

// Output data
out vec3 fragPosWorld;       // Position in world space
out vec3 color;
out vec2 uv;
out vec4 fragPosLightSpace;

// Matrices
uniform mat4 MVP;            // Model-View-Projection matrix
uniform mat4 LMAP;           // Light-space matrix
uniform mat4 modelMatrix;    // Model matrix for world-space transformation

void main() {
    // Transform vertex for main rendering
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass interpolated attributes to the fragment shader
    color = vertexColor;
    uv = vertexUV;

    // Light-space position for shadow calculation
    fragPosLightSpace = LMAP * vec4(vertexPosition, 1.0);

    // Transform to world space for voxel cone tracing
    fragPosWorld = vec3(modelMatrix * vec4(vertexPosition, 1.0));
}
