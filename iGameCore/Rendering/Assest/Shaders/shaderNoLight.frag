#version 410 core

//layout(std140, binding = 0) uniform MVPMatrix {
layout(std140) uniform MVPMatrix {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    mat4 mvp;// proj * view * model
} mvp;
//layout(std140, binding = 1) uniform UniformBufferObject {
layout(std140) uniform UniformBufferObject {
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