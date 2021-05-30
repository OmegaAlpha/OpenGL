#include <iostream>

#include "Renderer.h"
#include "Hex.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error!] (" << hex(error, 4) << ") " << function <<  ", " << line << std::endl; //printing padded hex value for searching error code
        return false;
    }

    return true;
}

void Renderer::Clear() const
{
    GLCallV(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();

    va.Bind();
    ib.Bind(); // Not strictly neccesary to bind this, the va already has a binding to the ib
    GLCallV(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
