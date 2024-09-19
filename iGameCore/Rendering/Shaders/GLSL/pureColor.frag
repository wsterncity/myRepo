#version 330 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec4 out_ScreenColor;

uniform int colorMode = 0;
uniform vec3 inputColor = vec3(1.0, 1.0, 1.0);

void main() {
    if (colorMode == 0) {
        out_ScreenColor = vec4(inputColor, 1.0);
    } else {
        out_ScreenColor = vec4(in_color, 1.0);
    }
}
