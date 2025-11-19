#include "GL_renderer.h"

namespace OpenGLRenderer {
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