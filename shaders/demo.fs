#version 330 core

// Interpolated values from the vertex shaders
in vec3 normal;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform vec3 LightPosition;

void main(){
	float lightShading = clamp(dot(normal, LightPosition), 0.0, 1.0);
	color = vec3(1.0, 0.41, 0.38) * lightShading;

}

