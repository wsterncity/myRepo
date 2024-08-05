#version 410 core

//layout(std140, binding = 0) uniform MVPMatrixBlock {
layout(std140) uniform MVPMatrixBlock {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    mat4 viewporj;// proj * view
} mvp;
//layout(std140, binding = 1) uniform UniformBufferObjectBlock {
layout(std140) uniform UniformBufferObjectBlock {
    vec3 viewPos;
    bool useColor;
} ubo;
//layout(binding = 2) uniform sampler2D texSampler;
uniform sampler2D texSampler;

in vec3 fragPosition;
in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}