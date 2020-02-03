#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>

/*
   使用openGL函数实例
   GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
   GLCall(glClear(GL_COLOR_BUFFER_BIT));
*/

#define ASSERT(x) if (!(x)) __debugbreak()
#define GLCall(x) GLClearError();\
   x;\
   ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
