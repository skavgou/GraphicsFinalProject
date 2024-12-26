#version 330 core

// Inputs from the vertex shader
in vec3 FragPos;        // Position of the fragment (in world space)
in vec3 Normal;         // Normal vector (in world space)
in vec2 TexCoord;       // Texture coordinates

// Output color
out vec4 FragColor;

// Lighting uniforms
uniform vec3 lightPos;    // Light position
uniform vec3 lightColor;  // Light color
uniform vec3 viewPos;     // Camera position

// Material properties
uniform vec4 baseColor;   // Base color of the material
uniform float metallic;   // Metallic factor
uniform float roughness;  // Roughness factor

void main() {
    // Ambient lighting
    vec3 ambient = 0.1 * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting (Phong reflection)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = lightColor * spec;

    // Combine results
    vec3 lighting = (ambient + diffuse + specular) * vec3(baseColor);

    // Output final color
    FragColor = vec4(lighting, baseColor.a);
}
