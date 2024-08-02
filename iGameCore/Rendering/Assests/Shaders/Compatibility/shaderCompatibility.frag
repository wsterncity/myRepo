uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 viewPos;
uniform bool useColor;

//uniform sampler2D texSampler;

varying highp vec3 fragPosition;
varying highp vec3 fragColor;
//varying highp vec3 fragNormal;
//varying highp vec2 fragTexCoord;

highp vec3 ambient = vec3(0.1, 0.1, 0.1);
struct Light {
    highp vec3 direction;
    highp vec3 color;
};

Light light = Light(
vec3(0.0, 0.0, -1.0),
vec3(1.0, 1.0, 1.0)
);

bool gamma = true;
vec3 BlinnPhong(highp vec3 normal, highp vec3 fragPos, Light light)
{
    // diffuse
    highp vec3 lightDir = normalize(-light.direction);
    highp float diff = max(dot(lightDir, normal), 0.0);
    highp vec3 diffuse = diff * light.color;
    // specular
    highp vec3 viewDir = normalize(viewPos - fragPos);
    highp float spec = 0.0;
    highp vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    highp vec3 specular = spec * light.color;

    return diffuse + specular;
}

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    highp vec3 color = vec3(0.0, 0.0, 0.0);

    // continuous patch
    //    highp vec3 normal = normalize(fragNormal);
    // discrete patch
    highp vec3 normal = normalize(cross(dFdx(fragPosition), dFdy(fragPosition)));

    // ambient
    color += ambient * fragColor;
    // lighting
    highp vec3 lighting = BlinnPhong(normal, fragPosition, light);
    color += lighting * fragColor;

    if (gamma) {
        color = pow(color, vec3(1.0 / 2.2));
    }
    gl_FragColor = vec4(color, 1.0);
}