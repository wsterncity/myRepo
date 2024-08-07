#version 410 core

layout(std140) uniform CameraDataBlock {
//layout(std140, binding = 0) uniform CameraDataBlock {
    mat4 view;
    mat4 proj;
    mat4 projview;// proj * view
} cameraData;

layout(std140) uniform ObjectDataBLock {
//layout(std140, binding = 1) uniform ObjectDataBLock {
    mat4 model;
    mat4 normal;// transpose(inverse(model))
    vec4 spherebounds;// not set now, do not use
} objectData;

layout(std140) uniform UniformBufferObjectBlock {
//layout(std140, binding = 2) uniform UniformBufferObjectBlock {
    vec3 viewPos;
    bool useColor;
} ubo;

//layout(binding = 3) uniform sampler2D texSampler;
uniform sampler2D texSampler;

in vec3 fragPosition;
in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 outColor;

vec3 ambient = vec3(0.4f, 0.4f, 0.4f);
struct Light {
    vec3 direction;
    vec3 color;
};
Light light = Light(
vec3(0.0f, 0.0f, -1.0f),
vec3(1.0f, 1.0f, 1.0f)
);

bool gamma = false;
vec3 BlinnPhong(vec3 normal, vec3 fragPos, Light light)
{
    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.color * 0.5f;
    // specular
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    float spec = 0.0;
    //    vec3 halfwayDir = normalize(lightDir + viewDir);
    //    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * light.color * 0.5f;

    return diffuse + specular;
    //    return diffuse;
}

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    vec3 color = vec3(0.0, 0.0, 0.0);

    // continuous patch
    //    vec3 normal = normalize(fragNormal);
    // discrete patch
    vec3 normal = normalize(cross(dFdx(fragPosition), dFdy(fragPosition)));

    // ambient
    color += ambient * fragColor;
    // lighting
    vec3 lighting = BlinnPhong(normal, fragPosition, light);
    color += lighting * fragColor;

    if (gamma) {
        color = pow(color, vec3(1.0 / 2.2));
    }
    outColor = vec4(color, 1.0);

    //outColor = vec4(fragColor, 1.0);
}