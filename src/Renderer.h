//
// Created by fox-1942 on 11/25/19.
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {

public:
    void Draw(const VertexArray &va, const IndexBuffer &ib, Shader shader);
};
#endif //OPENGL_RENDERER_H
