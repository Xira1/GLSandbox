#pragma once

#include <glad/glad.h>

#include "Common.h"
#include "../API/OpenGL/Types/GL_framebuffer.hpp"
#include "../API/OpenGL/Types/GL_shader.h"

struct Grid {
	GLuint VAO;

	void Init();
	void RenderGrid(OpenGLShader& shader, OpenGLFrameBuffer& fbo);
};