#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 fragTexCoord;

out vec4 outColor;

uniform uvec2 screenTexture;

float near = 0.1;
float far  = 10000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;// back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    outColor = texture(sampler2D(screenTexture), fragTexCoord);

    //    float level = 1.0;
    //    float depth = textureLod(sampler2D(screenTexture), fragTexCoord, level).r;
    //    float linearDepth = LinearizeDepth(depth) / far;
    //    outColor = vec4(vec3(linearDepth), 1.0);
}