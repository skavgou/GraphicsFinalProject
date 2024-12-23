#version 330 core

// Inputs
in vec3 fragPosWorld;
in vec3 color;
in vec2 uv;
in vec4 fragPosLightSpace;

// Outputs
out vec3 finalColor;

// Uniforms
uniform sampler2D textureSampler;
uniform sampler2D shadowDepthMap;
uniform sampler3D voxelTexture;  // Voxel grid for cone tracing
uniform vec3 lightDir;           // Direction of sunlight
uniform vec3 sunColor;           // Color of the sunlight
uniform vec3 cameraPos;          // Camera position (for specular reflections)

// Helper: Calculate shadow
float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1]
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    return currentDepth - bias > closestDepth ? 0.5 : 1.0; // Softer shadow
}

// Helper: Voxel cone trace for indirect lighting
vec3 voxelConeTrace(vec3 origin, vec3 direction, float coneAngle, int steps) {
    vec3 result = vec3(0.0);
    vec3 step = direction * coneAngle;

    for (int i = 0; i < steps; ++i) {
        origin += step;
        vec3 voxelColor = texture(voxelTexture, origin).rgb;
        result += voxelColor / float(steps); // Accumulate lighting
    }
    return result;
}

void main() {
    // Sample the base texture color
    vec3 textureColor = texture(textureSampler, uv).rgb;

    // Direct sunlight (lambertian shading)
    vec3 normLightDir = normalize(lightDir);
    float diff = max(dot(normLightDir, fragPosWorld), 0.0);
    vec3 directLighting = diff * sunColor * textureColor;

    // Calculate shadow
    float shadow = calculateShadow(fragPosLightSpace);

    // Indirect lighting using voxel cone tracing
    vec3 indirectLighting = voxelConeTrace(fragPosWorld, normLightDir, 0.1, 16);

    // Combine direct and indirect lighting
    vec3 ambient = 0.1 * textureColor; // Basic ambient light
    vec3 totalLighting = shadow * directLighting + indirectLighting + ambient;

    // Specular highlights (Blinn-Phong model)
    vec3 viewDir = normalize(cameraPos - fragPosWorld);
    vec3 halfDir = normalize(normLightDir + viewDir);
    float spec = pow(max(dot(halfDir, fragPosWorld), 0.0), 16.0);
    vec3 specular = spec * sunColor;

    // Final color
    finalColor = totalLighting + specular;
}
