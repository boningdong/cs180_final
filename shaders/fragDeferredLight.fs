#version 330 core
out vec4 fragColor;

in vec2 texcoords;

// these are textures output by the geometry pass.
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

struct Light {
    vec3 position; 
    vec3 color;
};

const int NR_LIGHTS = 50;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main() {
    vec3 fragPos = texture(gPosition, texcoords).rgb;
    vec3 normal = texture(gNormal, texcoords).rgb;
    vec3 color = texture(gColorSpec, texcoords).rgb;
    float specular = texture(gColorSpec, texcoords).a;

    // calculate lighting
    vec3 ambient = color * 0.1;
    vec3 lighting = ambient;
    vec3 viewDir = normalize(viewPos - fragPos);
    for (int i = 0; i < NR_LIGHTS; i++) {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * color * lights[i].color;
        lighting += diffuse;
        // specular
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect_dir(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16) * specular * lights[i].color;
        lighting += spec;
    }
    
    fragColor = vec4(lighting, 1.0);
}