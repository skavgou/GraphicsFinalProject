#version 330 core

in vec3 color;
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 UV;
out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D textureSampler;
uniform mat4 LMAP;
uniform sampler2D shadowDepthMap;

float calculateShadow(vec4 shadowCoord)
{
    // Perform perspective divide to get normalized device coordinates in shadow space
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;

    // Map to [0, 1] range for texture lookup
    projCoords = projCoords * 0.5 + 0.5;

    //if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0){
    //        return 0.2;
    //    }

    // Sample the depth from the shadow map
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Basic shadow comparison
    float shadow = (currentDepth >= closestDepth + 1e-3) ? 0.2 : 1.0;

    return shadow;
}

void main()
{
    vec3 texColor = texture(textureSampler, UV).rgb;
    // Calculate the light direction
    vec3 lightDir = normalize(lightPosition - worldPosition);

    float distance = length(lightPosition - worldPosition);
    //finalColor = texColor; return;
    //finalColor = worldNormal; return;
    //finalColor = vec3(distance / 1000.0f); return;
    // Calculate the diffuse component using Lambert's cosine law
    float diff = max(dot(worldNormal, lightDir), 0.0) / pow(distance, 2);

    // Calculate the final color with light intensity
    vec3 diffuse = diff * lightIntensity * texColor;

    // Calculate shadow coordinate by transforming the world position with the light's MVP matrix
    vec4 shadowCoord = LMAP * vec4(worldPosition, 1.0);

    // Calculate the shadow factor
    float shadowFactor = calculateShadow(shadowCoord);

    shadowFactor = (worldPosition.y >= lightPosition.y) ? 1.0 : shadowFactor;
    //float shadowFactor = 1.0;
    // Clamp the final color to prevent excessively high values
    finalColor = diffuse / (1.0 + diffuse);

    // Apply gamma correction
    float gamma = 2.2;
    finalColor = pow(finalColor, vec3(1.0 / gamma)) * shadowFactor;
}