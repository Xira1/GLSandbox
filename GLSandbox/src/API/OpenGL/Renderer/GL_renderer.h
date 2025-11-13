#pragma once

#include "../Types/GL_framebuffer.h"
#include "../Types/GL_shader.h"
#include "../Types/GL_cubemapView.h"
#include "../Types/GL_detachedMesh.hpp"
#include "../Camera/Camera.h"
#include "../AssetManagement/AssetManager.h"
#include "../GL_backend.h"

struct OpenGLRasterizerState {
	GLboolean depthTestEnabled = true;
	GLboolean blendEnabled = false;
	GLboolean cullfaceEnabled = true;
	GLboolean depthMask = true;
	GLfloat pointSize = 1.0f;
	GLenum blendFuncSrcfactor = GL_SRC_ALPHA;
	GLenum blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
	GLenum dethFunc = GL_LESS;
};

namespace OpenGLRenderer {
	void InitMain();
	void Init();
	void RenderFrame();
	void LoadShaders();

	// Passes
	void GridPass();
	void SkyBoxPass();

	void InitClouds();
	void CloudsPass();

	// Utils
	OpenGLCubemapView* GetCubemapView(const std::string& name);
	OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);
	OpenGLShader* GetShader(const std::string& name);
	OpenGLTexture* GetTexture(const std::string& name);
	Resolutions& GetResolutions();

	// Rasterizer states
	void InitRasterizerState();
	OpenGLRasterizerState* CreateRasterizerState(const std::string& name);
	OpenGLRasterizerState* GetRasterizerState(const std::string& name);
	void SetRasterizerState(const std::string& name);

	// Frame buffers
	void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);


	// Debug another day
	
	//
}

