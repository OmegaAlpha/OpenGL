#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdlib.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#ifdef _DEBUG
#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCallV( x ) \
     GLClearError(); \
     x; \
     ASSERT( GLCheckError( #x, __FILE__, __LINE__) )

#define GLCall( x ) [&]() { \
     GLClearError(); \
     auto retVal = x; \
     ASSERT( GLCheckError( #x, __FILE__, __LINE__) ) \
     return retVal; \
   }()

#else
#define GLCallV( x ) x
#define GLCall( x ) x

#define ASSERT(x) if(!(x)) {exit(1);} //glfwSetWindowShouldClose(window, true);}
#endif


void GLClearError();

bool GLCheckError(const char* function, const char* file, int line);



class Renderer
{
private:

public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

};