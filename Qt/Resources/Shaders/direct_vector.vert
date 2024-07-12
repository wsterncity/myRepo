#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 color;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
out vec3 FragPos;
out vec3 normal;
out mat4 Projection;
out VS_OUT {
   vec3 normal;
   mat4 MVP;
   vec3 old_normal;
} vsout;


void main()
{   
    Projection = projection;
    vsout.old_normal=color;
    normal = normalize(vec3(vec4(mat3(transpose(inverse(view * model))) * aNormal, 0.0)));
    vsout.MVP = projection * view * model;
    vsout.normal = normal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(gl_Position);
}