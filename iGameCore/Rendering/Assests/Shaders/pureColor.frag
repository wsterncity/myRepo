#version 330 core

out vec4 outColor;
in vec3 fragColor;

uniform int colorMode = 0;
uniform vec3 inputColor = vec3(1.0, 1.0, 1.0);

void main() {
    if (colorMode == 1) outColor = vec4(fragColor, 1.0);
    else outColor = vec4(inputColor, 1.0);
}
