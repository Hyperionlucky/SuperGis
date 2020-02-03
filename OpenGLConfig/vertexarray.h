#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H


#include "vertexbuffer.h"

class VertexBufferLayout;

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void Bind() const;
    void Unbind() const;
    unsigned int getRendererID(){return rendererID;}

private:
    unsigned int rendererID = 0;
};
#endif // VERTEXARRAY_H
