#version 330 core

// Input attributes from the vertex buffer
layout(location = 0) in vec3 aPos;     // Vertex position
layout(location = 1) in vec3 aNormal;  // Vertex normal
layout(location = 2) in vec2 aTexCoord; // Texture coordinates (if available)

// Uniforms for transformations
uniform mat4 model;      // Model transformation matrix
uniform mat4 view;       // View transformation matrix
uniform mat4 projection; // Projection transformation matrix

// Pass data to the fragment shader
out vec3 FragPos;        // Position of the fragment (in world space)
out vec3 Normal;         // Normal vector (in world space)
out vec2 TexCoord;       // Texture coordinates (unchanged)

void main() {
    // Transform the vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Pass world-space position and normals to the fragment shader
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals properly
    TexCoord = aTexCoord;
}
