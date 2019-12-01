#version 460 core
out vec4 fragColor;

in vec2 v_TexCoord;

uniform sampler2D texture_diffuse1;

void main() {
fragColor= texture(texture_diffuse1, v_TexCoord);
}