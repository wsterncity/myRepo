#version 410 core

layout(std140) uniform CameraDataBlock {
//layout(std140, binding = 0) uniform CameraDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projview;// proj * view
} cameraData;

layout(std140) uniform ObjectDataBlock {
//layout(std140, binding = 1) uniform ObjectDataBLock {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    vec4 spherebounds;// not set now, do not use
} objectData;

layout(std140) uniform UniformBufferObjectBlock {
//layout(std140, binding = 2) uniform UniformBufferObjectBlock {
    vec3 viewPos;
    bool useColor;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

out vec3 fragPosition;
out vec3 fragColor;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
    // vec4 plane[6];
    // plane[0] = vec4(0.5, 0.0, 0.0, 0.0);
    // plane[1] = vec4(0.0, 0.5, 0.0, 0.0);
    // plane[2] = vec4(0.5, 0.0, 0.0, 0.0);
    // plane[3] = vec4(0.0, 0.5, 0.0, 0.0);
    // plane[4] = vec4(0.5, 0.0, 0.0, 0.0);
    // plane[5] = vec4(0.0, 0.5, 0.0, 0.0);
    //
    //
    // gl_ClipDistance[0] = dot(plane[0], vec4(inPosition, 1.0));
    // gl_ClipDistance[1] = dot(plane[1], vec4(inPosition, 1.0));
    // gl_ClipDistance[2] = dot(plane[2], vec4(inPosition, 1.0));
    // gl_ClipDistance[3] = dot(plane[3], vec4(inPosition, 1.0));
    // gl_ClipDistance[4] = dot(plane[4], vec4(inPosition, 1.0));
    // gl_ClipDistance[5] = dot(plane[5], vec4(inPosition, 1.0));

    gl_Position = cameraData.projview * objectData.model * vec4(inPosition, 1.0);
    fragPosition = vec3(objectData.model * vec4(inPosition, 1.0));
    if (ubo.useColor) {
        fragColor = inColor;
    } else {
        fragColor = vec3(1.0f, 1.0f, 1.0f);
    }
    fragNormal = mat3(objectData.normal) * inNormal;
    fragTexCoord = inTexCoord;
}