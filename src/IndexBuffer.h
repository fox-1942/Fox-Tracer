//
// Created by fox-1942 on 11/25/19.
//

#ifndef OPENGL_INDEXBUFFER_H
#define OPENGL_INDEXBUFFER_H

class IndexBuffer {
private:


    unsigned int RendererID;
    unsigned int m_Count; //how many indices
public:
    IndexBuffer(const void *data, unsigned int count);

    ~IndexBuffer();

    void Bind() const;

    void unBind() const;

    inline unsigned int GetCount() const { return m_Count; };
};


#endif //OPENGL_VERTEXBUFFER_H
