#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
// Skinning information
layout(location = 3) in ivec3 joint_indices;
layout(location = 4) in vec3 joint_weights;

const int MAX_JOINTS = 50; // No model can have more than 50 bones
const int MAX_WEIGHTS = 3; // Each vertex can have at most 3 bones

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 LightPosition;

uniform mat4 jointTransforms[MAX_JOINTS];

void main(){

    vec4 totalLocalPosition = vec4(0.0);
    vec4 totalNormal = vec4(0.0);

    for(int i = 0; i<MAX_WEIGHTS; i++){
        mat4 jointTansform = jointTransforms[joint_indices[i]];
        vec4 posePosition = jointTransform * vec4(vertexPosition_modelspace, 1.0);
        totalLocalPosition += posePosition * joint_weights[i];

        vec4 worldNormal = jointTransform * vec4(vertexNormal_modelspace, 1.0);
        totalNormal += worldNormal * joint_weights[i];
    }

    vertexPosition_modelspace = totalLocalPosition;
    vertexNormal_modelspace = totalNormal;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);


	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	UV = vertexUV;
}

