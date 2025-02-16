#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(int width, int height)
    : width(width), height(height), fbo(0), textureID(0), rbo(0)
{
    CreateFramebuffer();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureID);
    glDeleteRenderbuffers(1, &rbo);
}

void Framebuffer::CreateFramebuffer()
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create texture to store color buffer
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    // Create a Renderbuffer Object for depth & stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int newWidth, int newHeight)
{
    if (newWidth == width && newHeight == height)
        return;

    width = newWidth;
    height = newHeight;

    glDeleteTextures(1, &textureID);
    glDeleteRenderbuffers(1, &rbo);

    CreateFramebuffer();
}