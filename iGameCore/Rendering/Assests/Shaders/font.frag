#version 330 core

in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D fontTexture;
uniform vec3 textColor;

void main()
{
    float alpha = texture(fontTexture, fragTexCoord).r;
    outColor = vec4(textColor, alpha);
}