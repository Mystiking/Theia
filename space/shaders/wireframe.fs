
#version 330 core

// Interpolated values from the vertex shaders
in vec3 geo_normal;
in vec3 distanceToEdges;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform vec3 LightPosition;

float edgeSmoothing(float x) {
    return 2 * (-2 * x * x);
}

void main(){
	//float lightShading = clamp(dot(geo_normal, LightPosition), 0.0, 1.0);
    //float edgeShading = edgeSmoothing(min(distanceToEdges[0], min(distanceToEdges[1], distanceToEdges[2])));
    if (min(distanceToEdges[0], min(distanceToEdges[1], distanceToEdges[2])) < 0.005) {
        color = vec3(0, 0, 0);
    } else {
	    color = vec3(1.0, 0.41, 0.38);// * lightShading;
    }

}