#include "GL_texture.h"


OpenGLTexture::OpenGLTexture(const std::string& filePath) {
	LoadFromFile(filePath);
}

void OpenGLTexture::LoadFromFile(const std::string& filePath) {
	m_filePath = filePath;
	unsigned char* data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channelCount, 0);

	if (data) {
		switch (m_channelCount) {
		case 1:
			m_format = GL_RED;
			break;
		case 3:
			m_format = GL_RGB;
			break;
		case 4:
			m_format = GL_RGBA;
			break;
		default:
			m_format = GL_RGB;
		}
			
		glGenTextures(1, &m_handle);
		glBindTexture(GL_TEXTURE_2D, m_handle);
		glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		std::cout << "Loaded texture: " << m_filePath << std::endl;
	}
	else {
		std::cout << "Failed to load texture: " << m_filePath << std::endl;
		stbi_image_free(data);
	}
}

void OpenGLTexture::Bind(int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void OpenGLTexture::Unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint& OpenGLTexture::GetHandle() {
	return m_handle;
}

int OpenGLTexture::GetWidth() {
	return m_width;
}

int OpenGLTexture::GetHeight() {
	return m_height;
}
