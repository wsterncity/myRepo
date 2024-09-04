#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

out vec3 fragPosition;
out vec3 fragColor;
out vec3 fragNormal;

void main() {
    gl_Position = proj * view * model * vec4(inPosition, 1.0);
    fragPosition = vec3(model * vec4(inPosition, 1.0));
    fragColor = inColor;
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
}