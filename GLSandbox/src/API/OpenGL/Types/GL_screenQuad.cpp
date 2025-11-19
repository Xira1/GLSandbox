#include "GL_screenQuad.h"
#include "../GL_backend.h"

ScreenQuad::ScreenQuad(const char* fragmentPath) {
	initQuad();
	m_shader = new OpenGLShader({ "screen.vert", fragmentPath });
}

OpenGLShader* const ScreenQuad::GetShaderPtr() {
	return m_shader;
}

void ScreenQuad::Draw() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ScreenQuad::initQuad() {
	float vertices[] = {
			-1.0f, -1.0f, 0.0, 0.0,
			1.0f, -1.0f, 1.0, 0.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f,  1.0f, 1.0, 1.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f, -1.0f, 1.0, 0.0
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}