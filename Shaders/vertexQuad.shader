#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform struct{
    mat4 projectionMatrix;
    mat4 modelMatrix;
} matricesQuad;

void main()
{
    gl_Position = matricesQuad.projectionMatrix * matricesQuad.modelMatrix * vec4(position, 1.0);

}


