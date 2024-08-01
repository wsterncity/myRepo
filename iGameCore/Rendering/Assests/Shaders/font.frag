//#version 410 core
//#extension GL_ARB_bindless_texture : require
//
//in vec2 fragTexCoord;
//
//out vec4 outColor;
//
//uniform uvec2 textureHandle;
//uniform vec3 textColor;
//
//void main()
//{
//    float alpha = texture(sampler2D(textureHandle), fragTexCoord).r;
//    outColor = vec4(textColor, alpha);
//}

#version 410 core

in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D fontTexture;
uniform vec3 textColor;

void main()
{
    float alpha = texture(fontTexture, fragTexCoord).r;
    outColor = vec4(textColor, alpha);
}