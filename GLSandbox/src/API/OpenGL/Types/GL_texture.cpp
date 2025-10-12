#include "GL_texture.h"

#include "DDS/DDS_Helpers.h"
#include <stb_image.h>
#include "../Types/TextureTools/TextureTools.h"
#include "../Utils/GL_utils.hpp"

//void OpenGLTexture::Load(const FileInfo& fileInfo, ImageDataType imageDataType) {
//
//}
//
//void OpenGLTexture::AllocateTextureMemory(int width, int height, int format, int internalFormat, int mimapLevelCount) {
//
//}

void OpenGLTexture::Bind(unsigned int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

GLuint& OpenGLTexture::GetHandle() {
	return m_handle;
}

void OpenGLTexture::SetWrapMode(TextureWrapMode wrapMode) {
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLUtils::TextureWrapModeToGLEnum(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLUtils::TextureWrapModeToGLEnum(wrapMode));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::SetMinFilter(TextureFilter filter) {
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLUtils::TextureFilterToGLEnum(filter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::SetMagFilter(TextureFilter filter) {
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLUtils::TextureFilterToGLEnum(filter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

int OpenGLTexture::GetWidth() {
	return m_width;
}

int OpenGLTexture::GetHeight() {
	return m_height;
}

int OpenGLTexture::GetChannelCount() {
	return m_channelCount;
}

void* OpenGLTexture::GetData() {
	return m_data;
}

GLint OpenGLTexture::GetFormat() {
	return m_format;
}

GLint OpenGLTexture::GetInternalFormat() {
	return m_internalFormat;
}

GLsizei OpenGLTexture::GetDataSize() {
	return m_dataSize;
}
