#pragma once
#include "../Common/Types.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

struct OpenGLTexture {
public:
	OpenGLTexture() = default;
	unsigned int Texture3D(float w, float h, float d);
	unsigned int Texture2D(float w, float h);
	void Bind(unsigned int slot);
	void SetWrapMode(TextureWrapMode wrapMode);
	void SetMinFilter(TextureFilter filter);
	void SetMagFilter(TextureFilter filter);
	int GetWidth();
	int GetHeight();
	int GetChannelCount();
	void* GetData();
	GLint GetFormat();
	GLint GetInternalFormat();
	GLsizei GetDataSize();
	GLuint& GetHandle();


private:
	GLuint m_handle = 0;
	GLuint64 m_bindlessID = 0;
	GLint m_mipmapLevelCount = 0;
	GLint m_format = 0;
	GLint m_internalFormat = 0;
	GLsizei m_dataSize = 0;
	void* m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channelCount = 0;
	ImageDataType m_imageDataType;
	bool m_memoryAllocated = false;
};

