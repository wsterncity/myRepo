#version 410 core

in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D screenTexture;

float near = 0.1;
float far  = 10000.0;

// depth range: 0.0(near plane) -> 1.0(far plane)
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;// back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    outColor = texture(screenTexture, fragTexCoord);

    // float level = 0.0;
    // float depth = textureLod(screenTexture, fragTexCoord, level).r;
    // float linearDepth = LinearizeDepth(depth) / far;
    // outColor = vec4(vec3(linearDepth), 1.0);
}