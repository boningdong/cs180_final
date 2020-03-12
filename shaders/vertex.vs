#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 raw_color;
layout (location = 2) in vec2 raw_tex_coord;

out vec3 color;
out vec2 tex_coord;

void main() {
    gl_Position = vec4(pos, 1.0);
    color = raw_color;
    tex_coord = raw_tex_coord;
}