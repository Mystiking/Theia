#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D diffuseTexture;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform float nearPlane;
uniform float farPlane;

uniform int has_texture;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Average over the surrounding pixels to create an anti-alias effect (i.e. remove jagged edges)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec3 color;
    if (has_texture == 1) {
        color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    } else {
	    color = vec3(1.0, 0.41, 0.38);
    }
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.6 * color;
    // diffuse
    //vec3 lightDirection = normalize(lightPosition - fs_in.FragPos); // To shade as a point light
    vec3 lightDirection = normalize(lightPosition); // To shade as global illumination
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadow
    float shadowBias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.000); // Hack to cure shadow acne
    shadowBias = 0.004;
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, shadowBias);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse )) * color;

    FragColor = vec4(lighting, 1.0);
    if (shadow == 1) {
        //FragColor = vec4(1,0,0,1);
    }
}
