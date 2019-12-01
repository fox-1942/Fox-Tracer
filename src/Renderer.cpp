//
// Created by fox-1942 on 11/25/19.
//


#include "Shader.h"
#include "Renderer.h"

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, Shader shader) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);

}
