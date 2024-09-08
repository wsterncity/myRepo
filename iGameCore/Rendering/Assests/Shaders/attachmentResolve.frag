#version 330 core

in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;// Output to color attachment 0 (GL_RGBA8 format)
layout(location = 1) out vec4 outDepth;// Output to color attachment 1 (GL_R32F format)

uniform int numSamples;
uniform sampler2DMS colorTextureMS;
uniform sampler2DMS depthTextureMS;

void main()
{
    ivec2 texSize = textureSize(colorTextureMS);
    ivec2 texCoord = ivec2(fragTexCoord * vec2(texSize));

    // color resolve(from multisamples to single sample)
    {
        ivec2 texSize = textureSize(colorTextureMS);
        ivec2 texCoord = ivec2(fragTexCoord * vec2(texSize));

        vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < numSamples; ++i) {
            vec4 sampleColor = texelFetch(colorTextureMS, texCoord, i);
            color += sampleColor;
        }
        outColor = color / float(numSamples);
        outDepth = vec4(color.r);
    }

    // depth resolve(from multisamples to single sample)
    {
        ivec2 texSize = textureSize(depthTextureMS);
        ivec2 texCoord = ivec2(fragTexCoord * vec2(texSize));

        float minDepth = 1.0;
        for (int i = 0; i < numSamples; ++i) {
            float sampleDepth = texelFetch(depthTextureMS, texCoord, i).r;
            minDepth = min(minDepth, sampleDepth);
        }
        outDepth  = vec4(minDepth, minDepth, minDepth, 1.0f);
    }
}