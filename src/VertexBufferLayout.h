//
// Created by fox-1942 on 11/25/19.
//

#ifndef OPENGL_VERTEXBUFFERLAYOUT_H
#define OPENGL_VERTEXBUFFERLAYOUT_H

#include <vector>
#include <GL/glew.h>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    bool normalized;
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout()
            : m_Stride(0) {}

    template<typename T>
    void Push(int count) {
        static_assert(false);
    }

    template<>
    void Push<float>(int count) {
        m_Elements.push_back({GL_FLOAT, static_cast<unsigned int>(count), false});
        m_Stride += sizeof(GL_FLOAT);
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }

    inline unsigned int GetStride() const { return m_Stride; }

};


#endif //OPENGL_VERTEXBUFFERLAYOUT_H
