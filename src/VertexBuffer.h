//
// Created by fox-1942 on 11/25/19.
//

#ifndef OPENGL_VERTEXBUFFER_H
#define OPENGL_VERTEXBUFFER_H

class VertexBuffer {
private:
    unsigned int RendererID;
public:
    VertexBuffer(const void *data, unsigned int size);

    ~VertexBuffer();

    void Bind() const;

    void unBind() const;


};


#endif //OPENGL_VERTEXBUFFER_H
