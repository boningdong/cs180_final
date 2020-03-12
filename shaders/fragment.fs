#version 330 core

in vec3 color;
in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D texture;

void main() {
    frag_color = texture(texture, tex_coord);
}