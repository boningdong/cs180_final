#version 330 core
out vec4 frag_color;

in vec2 texcoords;

// these are textures output by the geometry pass.
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

struct Light {
    vec3 position; 
    vec3 color;
};

const int NR_LIGHTS = 5;
uniform Light lights[NR_LIGHTS];
uniform vec3 view_pos;

void main() {
    vec3 frag_pos = texture(gPosition, texcoords).rgb;
    vec3 normal = texture(gNormal, texcoords).rgb;
    vec3 color = texture(gColorSpec, texcoords).rgb;
    float specular = texture(gColorSpec, texcoords).a;

    // calculate lighting
    vec3 ambient = color * 0.1;
    vec3 lighting = ambient;
    vec3 view_dir = normalize(view_pos - frag_pos);
    for (int i = 0; i < NR_LIGHTS; i++) {
        // diffuse
        vec3 light_dir = normalize(lights[i].position - frag_pos);
        vec3 diffuse = max(dot(normal, light_dir), 0.0) * color * lights[i].color;
        lighting += diffuse;
        // specular
        vec3 view_dir = normalize(view_pos - frag_pos);
        vec3 reflect_dir = reflect(-light_dir, normal);
        vec3 spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16) * specular * lights[i].color;
        lighting += spec;
    }
    frag_color = vec4(lighting, 1.0);
}