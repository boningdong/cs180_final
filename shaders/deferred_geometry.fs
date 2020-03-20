#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec3 pos;
in vec3 normal;
in vec2 texcoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
    gPosition = pos;
    gNormal = normalize(normal);
    gColorSpec.rgb = texture(texture_diffuse1, texcoords).rgb;
    gColorSpec.a = texture(texture_specular1, texcoords).r;
}