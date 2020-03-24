#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 v_TexCoord;
out vec3 p;

uniform struct{
   mat4 projectionMatrix;
   mat4 viewMatrix;
   mat4 modelMatrix;
} matrices;

void main()
{
   gl_Position = matrices.projectionMatrix* matrices.viewMatrix*matrices.modelMatrix* vec4(position, 1.0);
   v_TexCoord=texCoord;
   //p = wLookAt + wRight * cCamWindowVertex.x + wUp * cCamWindowVertex.y;
}






