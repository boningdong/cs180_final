#version 330 core

// in vec2 tex_coord;
in vec3 fragPos;
in vec3 normal;

out vec4 fragColor;

uniform sampler2D obj_texture;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // frag_color = texture(obj_texture, tex_coord);
    // frag_color = vec4(0.5f, 0.5f, 0.6f, 1.0f);

    // ambient color
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // fragColor = vec4(ambient * objectColor, 1.0);

    // diffuse color
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    

    // Specular color
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0f);
}