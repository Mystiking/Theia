#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D renderedTexture;

in vec2 UV;

void main(){
	float dval = texture(renderedTexture, UV).r;
	color = vec4(vec3(dval), 1);//texture(renderedTexture, UV);
}
