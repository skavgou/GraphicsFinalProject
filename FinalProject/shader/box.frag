#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragToLight;
in vec2 fragUV;

uniform vec3 lightColor;        // Light color/intensity
uniform vec3 ambientColor;      // Ambient light color
uniform sampler2D textureSampler; // Object texture sampler
uniform sampler2D shadowDepthMap; // Shadow map sampler

uniform mat4 lightSpaceMatrix;  // Light-space transformation matrix

out vec4 fragColor;

float calculateShadow(vec4 fragLightSpacePos) {
    // Perform perspective division to get normalized device coordinates
    vec3 projCoords = fragLightSpacePos.xyz / fragLightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    // Read depth from the shadow map
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Calculate shadow bias to prevent acne
    float bias = max(0.05 * (1.0 - dot(normalize(fragNormal), normalize(fragToLight))), 0.005);
    float shadow = (currentDepth - bias) > closestDepth ? 0.5 : 1.0; // Simple shadow calculation
    return shadow;
}

void main() {
    // Normalize input vectors
    vec3 normalizedNormal = normalize(fragNormal);
    vec3 normalizedToLight = normalize(fragToLight);

    // Diffuse lighting
    float diffuse = max(dot(normalizedNormal, normalizedToLight), 0.0);

    // Shadow calculation
    vec4 fragLightSpacePos = lightSpaceMatrix * vec4(fragPosition, 1.0);
    float shadow = calculateShadow(fragLightSpacePos);

    // Combine texture color with lighting
    vec4 textureColor = texture(textureSampler, fragUV);
    vec3 lighting = (ambientColor + shadow * diffuse * lightColor) * textureColor.rgb;

    fragColor = vec4(lighting, textureColor.a);
}
