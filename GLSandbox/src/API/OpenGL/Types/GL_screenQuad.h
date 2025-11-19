#pragma once

#include "GL_shader.h"
#include <glad/glad.h>


struct ScreenQuad {
public:
	ScreenQuad(const char* fragmentPath);

	OpenGLShader* const GetShaderPtr();

	void Draw();

private:
	OpenGLShader* m_shader;

	void initQuad();

	unsigned int VAO, VBO;
};