#version 330 core

in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D screenTexture;

float near = 0.01;
float far  = 300.0;

// depth range: 1.0(near plane) -> 0.0(far plane)
float ReverseZLinearizeDepth(float depth) {
    return near / depth;
}

void main()
{
    // color
    {
        outColor = texture(screenTexture, fragTexCoord);
    }

    // depth
    {
        // float depth = texture(screenTexture, fragTexCoord).r;
        // if (depth == 0.0f) {
        //     outColor = vec4(0.39215f, 0.58431f, 0.92941f, 1.0f);
        // } else {
        //     float linearDepth = ReverseZLinearizeDepth(depth) / far;
        //     outColor = vec4(vec3(linearDepth), 1.0);
        // }
    }

    // depth pyramid
    {
        // float level = 0.0;
        // float depth = textureLod(screenTexture, fragTexCoord, level).r;
        // if (depth == 0.0f) {
        //     outColor = vec4(0.39215f, 0.58431f, 0.92941f, 1.0f);
        // } else {
        //     float linearDepth = ReverseZLinearizeDepth(depth) / far;
        //     outColor = vec4(vec3(linearDepth), 1.0);
        // }
    }
}