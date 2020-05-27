#version 460 core
layout(location = 0) in vec2 normQuadCoord;

uniform vec3 viewPoint;
uniform vec3 canvasX, canvasY;
out vec3 pixel;

void main()
{
    pixel = canvasX * normQuadCoord.x + canvasY * normQuadCoord.y + viewPoint;
    gl_Position = vec4(normQuadCoord, 0, 1);
}