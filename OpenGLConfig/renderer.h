#ifndef RENDERER_H
#define RENDERER_H


#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"

class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
private:
};

#endif // RENDERER_H
