#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aPos;
layout (location = 3) in ivec3 aBoneIds;
layout (location = 4) in vec3 aBoneWeights;

// Values that stay constant for the whole mesh.
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat4 bone_transforms[50];
uniform int is_animated;

void main(){
    if (is_animated == 1) {
        gl_Position = vec4(0.0);
        for (int i = 0; i < 3; i++) {
            if (aBoneIds[i] != -1) {
	            gl_Position =  gl_Position + aBoneWeights[aBoneIds[i]] * lightSpaceMatrix * model * bone_transforms[aBoneIds[i]] * vec4(aPos, 1);
            }
        }
    } else {
	    gl_Position =  lightSpaceMatrix * model * vec4(aPos, 1);
    }
}
