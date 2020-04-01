#version 460 core

uniform sampler2D tex;

in vec2 texcoord;
out vec4 FragColor;

void main()
{
    //FragColor = texture(tex, texcoord);
    FragColor= vec4(0.0f, 0.0f, 1.0f,1.0f);

}

