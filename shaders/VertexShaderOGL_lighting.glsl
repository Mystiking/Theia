#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;


out vec3 ambient_lighting_normal;
out vec3 frag_position;
out vec3 object_color;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;

void main() {

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    frag_position = vec3(M * vec4(vertexPosition_modelspace, 1.0));
    ambient_lighting_normal = mat3(transpose(inverse(M))) * vertexNormal_modelspace;

    object_color = vertexColor;
}
