#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct OpenGLTexture3D {
public:
    OpenGLTexture3D() = default;
    void Create(int size, GLenum internalFormat);
    void Reset();

    GLuint GetSize() const { return m_size; }
    GLuint GetHandle() const { return m_handle; }
    GLenum GetInternalFormat() const { return m_internalFormat; }

private:
    GLuint m_handle;
    GLuint m_size = 0;
    GLenum m_internalFormat = 0;
};