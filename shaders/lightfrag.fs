#version 330 core

// in vec2 tex_coord;

out vec4 frag_color;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    frag_color = vec4(lightColor, 1.0);
}