#version 330 core

uniform vec4 global_color;
out vec4 frag_color;

void main() {
    frag_color = global_color;
}