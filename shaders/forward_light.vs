#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
    pos = vec3(model * vec4(in_pos, 1.0));
    normal = mat3(transpose(inverse(model))) * in_normal;
}