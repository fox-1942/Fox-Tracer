#version 460 core
layout(location = 0) in vec2 cCamWindowVertex;

uniform vec3 wLookAt, wRight, wUp;          // pos of eye
out vec3 p;

void main()
{
    gl_Position = vec4(cCamWindowVertex, 0, 1);
    p = wLookAt + wRight * cCamWindowVertex.x + wUp * cCamWindowVertex.y;
}