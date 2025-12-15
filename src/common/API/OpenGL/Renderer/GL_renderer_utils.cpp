#include "GL_renderer.h"

namespace OpenGLRenderer {
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    void InitQuad() {
        float vertices[] = {
            -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, 1.0, 0.0,
            -1.0f,  1.0f, 0.0, 1.0,
            1.0f,  1.0f, 1.0, 1.0,
            -1.0f,  1.0f, 0.0, 1.0,
            1.0f, -1.0f, 1.0, 0.0
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void DrawQuad() {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

	void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter) {
		GLint srcAttachmentSlot = srcFrameBuffer->GetColorAttachmentSlotByName(srcName);
		GLint dstAttachmentSlot = dstFrameBuffer->GetColorAttachmentSlotByName(dstName);

        if (srcAttachmentSlot != GL_INVALID_VALUE && dstAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer->GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer->GetHandle());
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(dstAttachmentSlot);
            BlitRect srcRect;
            srcRect.x0 = 0;
            srcRect.y0 = 0;
            srcRect.x1 = srcFrameBuffer->GetWidth();
            srcRect.y1 = srcFrameBuffer->GetHeight();
            BlitRect dstRect;
            dstRect.y0 = 0;
            dstRect.x0 = 0;
            dstRect.x1 = dstFrameBuffer->GetWidth();
            dstRect.y1 = dstFrameBuffer->GetHeight();
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
	}

    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* name, GLbitfield mask, GLenum filter) {
        GLint srcAttachmentSlot = srcFrameBuffer->GetColorAttachmentSlotByName(name);

        if (srcAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer->GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(GL_BACK);
            BlitRect srcRect;
            srcRect.x0 = 0;
            srcRect.y0 = 0;
            srcRect.x1 = srcFrameBuffer->GetWidth();
            srcRect.y1 = srcFrameBuffer->GetHeight();
            BlitRect dstRect;
            dstRect.y0 = 0;
            dstRect.x0 = 0;
            dstRect.x1 = OpenGLBackend::GetWindowWidth();
            dstRect.y1 = OpenGLBackend::GetWindowHeight();
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
    }
}