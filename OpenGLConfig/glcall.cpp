#include "glcall.h"

#include <string>
#include <QDebug>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        char errorMsg[512] = { 0 };
        sprintf(errorMsg, "[OpenGL Error] (0x%04x) %s %s %d", error, function, file, line);
        printf("%s\n", errorMsg);
        qDebug() << errorMsg;
    //	LError(errorMsg);
        return false;
    }
    return true;
}
