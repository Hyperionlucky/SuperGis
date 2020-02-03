#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

class VertexBuffer {
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();
    void Bind() const;
    void Unbind() const;
    void addSubData(const void* data, unsigned int offset, unsigned int size /*GL_STATIC_DRAW*/);
    unsigned int getRendererID(){return rendererID;}
    int getVerticeCount() { return count; }

private:
    unsigned int rendererID;
    int count = 0;
};



#endif // VERTEXBUFFER_H
