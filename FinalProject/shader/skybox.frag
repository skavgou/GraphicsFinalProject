#version 330 core

in vec3 color;

out vec3 finalColor;

// TODO: To receive UV input to this fragment shader
in vec2 uv;
// TODO: To access the texture sampler
uniform sampler2D textureSampler;

void main()
{
    vec3 textureColor = texture(textureSampler, uv).rgb;

    // If you want only the texture color without modulation, just use:
    finalColor = textureColor;

}