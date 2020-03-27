#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;


void main()
{
    gl_Position = projectionMatrix * modelMatrix * vec4(position, 1.0);
}


