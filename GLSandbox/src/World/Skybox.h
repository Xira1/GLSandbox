#pragma once

#include "glad/glad.h"
#include "Primitives.h"
#include "../API/OpenGL/Types/GL_shader.h"
#include "../API/OpenGL/Types/GL_framebuffer.hpp"

#include <stb_image.h>

struct CubemapTexture {
	GLuint ID;

	void Create(std::vector<std::string>& textures);
};

struct Skybox {
	GLuint VAO = 0;
	CubemapTexture cubemap;

	void Init();
	void RenderSkyBox(OpenGLShader& shader, OpenGLFrameBuffer& fbo);
};