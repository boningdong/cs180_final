#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 raw_tex_coord;
layout (location = 2) in vec3 aNormal;

// out vec3 color;
// out vec2 tex_coord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    // tex_coord = raw_tex_coord;
    fragPos = vec3(model * vec4(pos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;
}