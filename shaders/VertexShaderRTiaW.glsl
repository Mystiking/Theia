#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;


out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;

out vec3 fragmentColor;

void main() {

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    Position_worldspace = (M * vec4(vertexPosition_modelspace, 1.0)).xyz;
    vec3 vertexPosition_cameraSpace = (V * M * vec4(vertexPosition_modelspace, 1.0)).xyz;
    EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraSpace;

    vec3 LightPosition_cameraspace = (V * vec4(vertexPosition_modelspace, 1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;


    vec3 lightColor = vec3(1, 1, 1);
    float lightPower = 60;
    vec3 diffuse_color = vec3(0.5, 0.0, 0.0);

    float distance = length(lightPosition_worldspace - Position_worldspace);
    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    float cosTheta = clamp(acos(dot(n, l)), 0, 1);



    vec3 color = diffuse_color * lightColor * lightPower * cosTheta / (distance*distance);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;



    color = ambient * diffuse_color;
    fragmentColor = color;
}
