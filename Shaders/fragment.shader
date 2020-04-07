#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

layout(std140,binding=2) uniform primitives{
    vec3 primitiveCoordinates[3430];
};

void main()
{
    FragColor =vec4(primitiveCoordinates[2].x-576.0f, primitiveCoordinates[2].y-56.0f, primitiveCoordinates[2].z-704.0f, 1.0f);
}