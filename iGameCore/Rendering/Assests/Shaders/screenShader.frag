#version 410 core

in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2DMS screenTextureMS;
uniform sampler2D depthPyramid;
uniform int numSamples;

float near = 0.01;
float far  = 300.0;

// depth range: 1.0(near plane) -> 0.0(far plane)
float ReverseZLinearizeDepth(float depth) {
    return near / depth;
}

void main()
{
    ivec2 texSize = textureSize(screenTextureMS);
    ivec2 texCoord = ivec2(fragTexCoord * vec2(texSize));

    // color
    {
        vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < numSamples; ++i) {
            vec4 sampleColor = texelFetch(screenTextureMS, texCoord, i);
            color += sampleColor;
        }
        outColor = color / numSamples;
    }

    // depth
    {
        // float minDepth = 1.0;
        // for (int i = 0; i < numSamples; ++i) {
        //     float sampleDepth = texelFetch(screenTextureMS, texCoord, i).r;
        //     minDepth = min(minDepth, sampleDepth);
        // }
        //
        // if (minDepth == 0.0f) {
        //     outColor = vec4(0.39215f, 0.58431f, 0.92941f, 1.0f);
        // } else {
        //     float linearDepth = ReverseZLinearizeDepth(minDepth) / far;
        //     outColor = vec4(vec3(linearDepth), 1.0);
        // }
    }

    // depth pyramid
    {
        // float level = 0.0;
        // float depth = textureLod(depthPyramid, fragTexCoord, level).r;
        // if (depth == 0.0f) {
        //     outColor = vec4(0.39215f, 0.58431f, 0.92941f, 1.0f);
        // } else {
        //     float linearDepth = ReverseZLinearizeDepth(depth) / far;
        //     outColor = vec4(vec3(linearDepth), 1.0);
        // }
    }
}