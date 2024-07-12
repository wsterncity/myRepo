#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 60) out;
in VS_OUT {
   vec3 normal;
   mat4 MVP;
   vec3 old_normal;
} gs_in[];
in mat4 Projection[];
uniform float scale;
out vec3 fColor;

void Generate(int index, vec3 color)
{
    vec3 diff=vec3(0.65,0.65,0.65);
    vec3 ambient=vec3(1.0,1.0,1.0);
    fColor = vec3(gs_in[index].old_normal  * diff + ambient * 0.2);
    //fColor = gs_in[index].old_normal ;
    vec3 L=gs_in[index].normal;
    vec3 normal1 = cross(vec3(0.0, 1.0, 0.0), L);
    vec3 normal2 = cross(normal1, L);
    vec4 normal=Projection[index] * vec4(gs_in[index].normal, 0.0);//ת����ķ�����
    vec4 center_high=gl_in[index].gl_Position+normal*0.15*scale;//��ߵ�
    vec4 vertices[7];
    vec4 vertices_mid[7];
    vec4 vertices_high[7];
    for (int i = 0; i < 7; ++i) {
        float angle = radians(60.0 * float(i));
        vec4 tem=Projection[index] * vec4(normalize(normal1 * cos(angle) + normal2 * sin(angle)), 0.0);
       vec4 vertex = gl_in[index].gl_Position +tem*0.01*scale ;
        vertices[i] = vertex;
        vertices_mid[i]=vertex+normal*0.09*scale;
        vertices_high[i]=gl_in[index].gl_Position + tem*0.03*scale+ normal * 0.09*scale ;
    }
    //������
    gl_Position = vertices[0];
     EmitVertex();
    gl_Position = vertices[1];
     EmitVertex();
    gl_Position = vertices[5];
     EmitVertex();
    gl_Position = vertices[2];
     EmitVertex();
    gl_Position = vertices[4];
     EmitVertex();
    gl_Position = vertices[3];
     EmitVertex();
    EndPrimitive();
    //���м�
    for(int i=0;i<6;++i){
    gl_Position = vertices[i];
     EmitVertex();
    gl_Position = vertices[(i+1)%6];
     EmitVertex();
    gl_Position = vertices_mid[i];
     EmitVertex();
    gl_Position = vertices_mid[(i+1)%6];
     EmitVertex();
     EndPrimitive();
    }
   //��ǰ��
       for(int i=0;i<6;i=i+3){
    gl_Position = vertices_high[i];
     EmitVertex();
    gl_Position = vertices_high[(i+1)%6];
     EmitVertex();
    gl_Position = center_high;
     EmitVertex();
    gl_Position = vertices_high[(i+2)%6];
     EmitVertex();
    gl_Position = vertices_high[(i+3)%6];
     EmitVertex();
     EndPrimitive();
    }
}
void main() {    
    Generate(0, vec3(1.0, 1.0, 0.0));
    Generate(1, vec3(1.0, 1.0, 0.0));
    Generate(2, vec3(1.0, 1.0, 0.0));
}