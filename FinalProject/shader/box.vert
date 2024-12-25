#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV; // Texture coordinates

uniform mat4 MVP;            // Model-View-Projection matrix
uniform mat4 modelMatrix;    // Model matrix
uniform vec3 lightPosition;  // Dynamic light position in world space

out vec3 fragNormal;         // Normal vector in world space
out vec3 fragPosition;       // Position of the fragment in world space
out vec2 fragUV;             // Interpolated texture coordinates
out vec3 fragToLight;        // Vector to light source

void main() {
    vec4 worldPosition = modelMatrix * vec4(vertexPosition_modelspace, 1.0);
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(transpose(inverse(modelMatrix))) * vertexNormal_modelspace;
    fragUV = vertexUV;
    fragToLight = lightPosition - fragPosition;

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}
