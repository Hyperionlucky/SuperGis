#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer {
public:
    IndexBuffer(const unsigned int* data, unsigned int count, unsigned int mode);
    ~IndexBuffer();
    void Bind() const;
    void Unbind() const;

    unsigned int getMode() const { return mode; }

    inline unsigned int getCount() const { return count; }
private:
    unsigned int rendererID;
    unsigned int count;
    unsigned int mode;
};



#endif // INDEXBUFFER_H
