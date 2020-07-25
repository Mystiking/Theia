#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec3 normal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

void main(){
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	normal = vertexNormal_modelspace.xyz;
}


