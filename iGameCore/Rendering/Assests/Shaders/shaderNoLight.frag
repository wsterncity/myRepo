#version 410 core

layout(std140) uniform CameraDataBlock {
//layout(std140, binding = 0) uniform CameraDataBlock {
    mat4 view;
    mat4 proj;
    mat4 proj_view;// proj * view
} cameraData;

layout(std140) uniform ObjectDataBlock {
//layout(std140, binding = 1) uniform ObjectDataBLock {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    vec4 sphereBounds;// not set now, do not use
} objectData;

layout(std140) uniform UniformBufferObjectBlock {
//layout(std140, binding = 2) uniform UniformBufferObjectBlock {
    vec3 viewPos;
    bool useColor;
} ubo;

//layout(binding = 3) uniform sampler2D texSampler;
uniform sampler2D texSampler;

in vec3 fragPosition;
in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}