#version 410 core

uniform vec3 viewPos;

in vec3 fragPosition;
in vec3 fragColor;
in vec3 fragNormal;

out vec4 outColor;

vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
struct Light {
    vec3 direction;
    vec3 color;
};
Light light = Light(
vec3(0.0f, 0.0f, -1.0f),
vec3(1.0f, 1.0f, 1.0f)
);

bool gamma = true;
vec3 BlinnPhong(vec3 normal, vec3 fragPos, Light light)
{
    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.color;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color;
    return diffuse + specular;
}

void main() {
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

    // outColor = vec4(fragColor, 1.0);
}