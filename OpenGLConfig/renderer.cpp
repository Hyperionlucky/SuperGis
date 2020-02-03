#include "renderer.h"
#include "glcall.h"

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
    GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    if (ib.getMode() == GL_POINTS) {
        glPointSize(4);
    }
    GLCall(glDrawElements(ib.getMode(), ib.getCount(), GL_UNSIGNED_INT, nullptr));
    if (ib.getMode() == GL_POINTS) {
        glPointSize(1);
    }
}
