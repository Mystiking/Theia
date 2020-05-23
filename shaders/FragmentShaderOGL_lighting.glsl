#version 330 core
in vec3 ambient_lighting_normal;
in vec3 frag_position;
in vec3 object_color;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 lightPosition_worldspace;
uniform vec3 cameraPosition;

out vec3 color;

void main() {
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(ambient_lighting_normal);
    vec3 light_direction = normalize(lightPosition_worldspace - frag_position);

    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 view_direction = normalize(cameraPosition - frag_position);
    vec3 reflect_direction = reflect(-ambient_lighting_normal, norm);

    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    color = (ambient + diffuse + specular) * object_color;
}
