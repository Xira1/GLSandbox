#pragma once

#include "../Types/GL_framebuffer.h"
#include "../Types/GL_shader.h"
#include "../Types/GL_cubemapView.h"
#include "Renderer/Types/TextureSet.h"
#include "../Types/GL_detachedMesh.hpp"
#include "Camera/Camera.h"
#include "AssetManagement/AssetManager.h"
#include "API/OpenGL/GL_backend.h"
#include "Common/CreateInfo.h"

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
	OpenGLTextureSet* GetTextureSet(const std::string& name);

	void InitQuad();
	void DrawQuad();

	// Rasterizer states
	void InitRasterizerState();
	OpenGLRasterizerState* CreateRasterizerState(const std::string& name);
	OpenGLRasterizerState* GetRasterizerState(const std::string& name);
	void SetRasterizerState(const std::string& name);

	// Frame buffers
	void ClearRenderTargets();
	void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);
	void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* name, GLbitfield mask, GLenum filter);

	// Debug another day
	
	//
}

