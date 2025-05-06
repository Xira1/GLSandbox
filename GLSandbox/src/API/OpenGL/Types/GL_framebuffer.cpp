#include "GL_framebuffer.h"

void OpenGLFramebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void OpenGLFramebuffer::SetViewport() {
	glViewport(0, 0, m_width, m_height);
}
