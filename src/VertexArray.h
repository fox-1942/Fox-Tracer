//
// Created by fox-1942 on 11/25/19.
//

#ifndef OPENGL_VERTEXARRAY_H
#define OPENGL_VERTEXARRAY_H

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {

private:
    unsigned int m_RendererID;

public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

    void Bind() const;

    void Unbind() const;

    void AddBuffer(const VertexBuffer &vb);
};

#endif //OPENGL_VERTEXARRAY_H
