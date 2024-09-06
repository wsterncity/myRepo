#version 330 core

layout (location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 fragTexCoord;

uniform mat4 proj;

void main()
{
    gl_Position = proj * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
}