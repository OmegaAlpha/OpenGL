#pragma once
#include <GL/glew.h>

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;
    GLuint GetTextureID() const { return textureID; }
    void Resize(int newWidth, int newHeight);

private:
    GLuint fbo;
    GLuint textureID;
    GLuint rbo;  // Renderbuffer for depth & stencil
    int width, height;

    void CreateFramebuffer();
};