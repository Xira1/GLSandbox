#pragma once
#include "../Common/Types.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

struct OpenGLTexture {
public:
	OpenGLTexture() = default;
	OpenGLTexture(const std::string& filePath);
	void LoadFromFile(const std::string& filePath);
	void Bind(int slot = 0) const;
	void Unbind() const;
	int GetWidth();
	int GetHeight();
	GLuint& GetHandle();

private:
	GLuint m_handle = 0;
	GLint m_mipmapLevelCount = 0;
	GLint m_format;
	int m_width = 0;
	int m_height = 0;
	int m_channelCount = 0;
	std::string m_filePath;
};

