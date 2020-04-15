#version 460 core

uniform sampler2D tex;

in vec2 texcoord;

out vec4 fragColor;

void main(void) {
     fragColor= texture2D(tex, texcoord);
}