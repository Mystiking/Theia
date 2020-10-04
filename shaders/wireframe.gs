#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 MVP;
uniform mat4 InverseP;

in vec3 normal[];
out vec3 geo_normal;

out vec3 distanceToEdges;

void main() {
    

    gl_Position = gl_in[0].gl_Position;
    geo_normal = normal[0];
    distanceToEdges = vec3(1, 0, 0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    geo_normal = normal[1];
    distanceToEdges = vec3(0, 1, 0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    geo_normal = normal[2];
    distanceToEdges = vec3(0, 0, 1);
    EmitVertex();
    EndPrimitive();
}