#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;


uniform struct{
   mat4 projectionMatrix;
   mat4 viewMatrix;
   mat4 modelMatrix;
} matrices;

void main()
{
   TexCoords = aTexCoords;
   gl_Position = matrices.projectionMatrix* matrices.viewMatrix*matrices.modelMatrix* vec4(aPos, 1.0);

   //p = wLookAt + wRight * cCamWindowVertex.x + wUp * cCamWindowVertex.y;
}

