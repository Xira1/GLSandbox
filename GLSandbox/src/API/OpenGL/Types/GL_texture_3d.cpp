#include "GL_texture_3d.h"

void OpenGLTexture3D::Create(int size, GLenum internalFormat) {
    if (m_handle != 0) {
        Reset();
    }

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_3D, m_handle);
    glTexStorage3D(GL_TEXTURE_3D, 1, internalFormat, size, size, size);

    m_internalFormat = internalFormat;
    m_size = size;
}

void OpenGLTexture3D::Reset() {
    glDeleteTextures(1, &m_handle);
}