uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 viewPos;
uniform bool useColor;

attribute vec3 inPosition;
attribute vec3 inColor;
attribute vec3 inNormal;
attribute vec2 inTexCoord;

varying vec3 fragPosition;
varying vec3 fragColor;
//varying vec3 fragNormal;
//varying vec2 fragTexCoord;

void main() {
    gl_Position = proj * view * model * vec4(inPosition, 1.0);

    fragPosition = vec3(model * vec4(inPosition, 1.0));
    if (useColor) {
        fragColor = inColor;
    } else {
        fragColor = vec3(0.5, 0.5, 0.5);
    }
    //    fragNormal = mat3(transpose(inverse(model))) * inNormal;
    //    fragTexCoord = inTexCoord;
}