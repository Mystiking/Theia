#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aPos;

// Values that stay constant for the whole mesh.
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main(){
	gl_Position =  lightSpaceMatrix * model * vec4(aPos, 1);
}