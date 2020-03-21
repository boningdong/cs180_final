#version 330 core

in vec3 pos;
in vec3 normal;

uniform vec3 light_color;

out vec4 frag_color;

void main() {
    frag_color = vec4(light_color, 1.0);
}