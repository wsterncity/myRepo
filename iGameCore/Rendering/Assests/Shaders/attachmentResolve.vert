#version 330 core

out vec2 fragTexCoord;

void main()
{
    // index0, x: 0<<1=00, 00&10=00, y: 00&10=0, the coordinate corresponding to vertex0 is (0,0)
    // index1, x: 1<<1=10, 10&10=10, y: 01&10=0, the coordinate corresponding to vertex0 is (2,0)
    // index2, x: 2<<1=100, 100&010=000, y:10&10=10, the coordinate corresponding to vertex0 is (0,2)
    fragTexCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(fragTexCoord * 2.0f - 1.0f, 0.0f, 1.0f);
}