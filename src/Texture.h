//
// Created by fox-1942 on 11/29/19.
//

#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Renderer.h"
#include <string>

class Texture {
public:
    Texture();

    unsigned int m_RendererID;
    std::string path;
    std::string type;
    std::string m_FilePath;
    unsigned char *m_LocalBuffer;
    int m_Width, m_Height, m_BPP;

public:
    Texture(const std::string &path);

    ~Texture();

    void Bind(unsigned int slot = 0) const;

    void Unbind();

    inline int GetWidth() const {
        return m_Width;
    }

    inline int GetHeight() const {
        return m_Height;
    }

};


#endif //OPENGL_TEXTURE_H
