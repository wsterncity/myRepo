#version 410 core

uniform mat4 mvp;

layout(location = 0) in vec3 inPosition;

out vec3 vertPosition;

void main() {
    vertPosition = inPosition;
    gl_Position = mvp * vec4(inPosition, 1.0);
}