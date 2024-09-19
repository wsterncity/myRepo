#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertPosition[];

out vec3 vertBarycentric;

void main() {
    // Compute barycentric coordinates for the triangle vertices
    vec3 barycentricCoords[3] = vec3[](vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        vertBarycentric = barycentricCoords[i];// Pass barycentric coordinate
        EmitVertex();
    }
    EndPrimitive();
}
