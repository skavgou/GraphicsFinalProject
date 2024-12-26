#version 330 core

in vec4 fragPosLightSpace;
in vec2 TexCoords;

uniform sampler2D textureSampler;
uniform sampler2D shadowDepthMap;

float calculateShadow(vec4 fragPosLightSpace) {
    // Transform to normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Retrieve the closest depth from the shadow map
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Bias to prevent shadow acne
    float bias = 0.005;
    return (currentDepth - bias > closestDepth) ? 0.5 : 1.0; // Shadow factor
}

void main() {
    float shadow = calculateShadow(fragPosLightSpace);
    vec4 textureColor = texture(textureSampler, TexCoords);
    gl_FragColor = vec4(textureColor.rgb * shadow, textureColor.a);
}
