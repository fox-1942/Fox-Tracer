#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform int valami[];

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}