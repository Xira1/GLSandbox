#pragma once
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <cstring>

class OpenGLFramebuffer {
private:
	GLuint m_handle = 0;
	GLuint m_width = 0;
	GLuint m_height = 0;

public:
	void Bind();
	void SetViewport();

};

