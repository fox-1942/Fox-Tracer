#version 430 core
uniform sampler2D tex;

/* This comes interpolated from the vertex shader */
in vec2 texcoord;

out vec4 color;

void main(void) {
     /* Well, simply sample the texture */
     color = texture2D(tex, texcoord);
}
