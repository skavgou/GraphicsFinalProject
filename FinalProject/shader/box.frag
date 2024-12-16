#version 330 core

// Inputs from the vertex shader
in vec3 color;
in vec2 uv;                // Interpolated UVs
in vec4 fragPosLightSpace; // Light-space position

// Outputs
out vec3 finalColor;

// Uniforms
uniform sampler2D textureSampler;     // Texture sampler
uniform sampler2D shadowDepthMap;     // Shadow map sampler

// Shadow calculation helper
float calculateShadow(vec4 fragPosLightSpace) {
    // Perform perspective divide to get normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range for texture lookup
    projCoords = projCoords * 0.5 + 0.5;

    // Depth value in shadow map
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;

    // Current fragment depth in light space
    float currentDepth = projCoords.z;

    // Bias to reduce shadow acne
    float bias = 0.005;

    // Check if the current fragment is in shadow
    float shadow = currentDepth - bias > closestDepth ? 0.5 : 1.0; // 0.5 for softer shadows
    return shadow;
}

void main() {
    // Sample the texture
    vec3 textureColor = texture(textureSampler, uv).rgb;

    // Compute shadow factor
    float shadow = calculateShadow(fragPosLightSpace);

    // Final color: texture modulated by shadow
    finalColor = shadow * textureColor;
}
