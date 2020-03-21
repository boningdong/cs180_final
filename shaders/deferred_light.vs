#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texcoords;

out vec2 texcoords;

void main() {
    gl_Position = vec4(in_pos, 1.0);
    texcoords = in_texcoords;
}