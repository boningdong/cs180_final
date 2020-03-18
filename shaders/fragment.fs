#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 texcoords;

uniform vec3 light_pos;
uniform vec3 view_pos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

out vec4 frag_color;

void main() {
    // light color
    vec3 light_diffuse = vec3(1.0f, 1.0f, 1.0f);
    vec3 light_ambient = vec3(0.2f, 0.2f, 0.2f);
    vec3 light_specular = vec3(1.0f, 1.0f, 1.0f);

    // diffuse color
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light_diffuse * diff * vec3(texture(texture_diffuse1, texcoords));
    vec3 ambient = light_ambient * vec3(texture(texture_diffuse1, texcoords));

    // Specular color
    vec3 view_dir = normalize(view_pos - pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = light_specular * spec * vec3(texture(texture_specular1, texcoords));
    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0f);
    // frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}