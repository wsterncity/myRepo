#version 410 core

layout(std140) uniform MVPMatrix {
//layout(std140, binding = 0) uniform MVPMatrix {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    mat4 mvp;// proj * view * model
} mvp;

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = mvp.mvp * vec4(inPosition, 1.0);
}
