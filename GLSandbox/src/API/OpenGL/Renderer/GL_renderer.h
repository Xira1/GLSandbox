#pragma once

#include "../Types/GL_framebuffer.hpp"
#include "../Types/GL_shader.h"
#include "../Types/GL_cubemapView.h"
#include "../Types/GL_detachedMesh.hpp"
#include "../Camera/Camera.h"
#include "../AssetManagement/AssetManager.h"
#include "../GL_backend.h"

namespace OpenGLRenderer {
	void InitMain();
	void Init();
	void RenderFrame();
	void LoadShaders();

	// Passes
	void SkyBoxPass();
	void CloudsPass();
	void GridPass();

	// Utils
	OpenGLCubemapView* GetCubemapView(const std::string& name);
	OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);
	OpenGLShader* GetShader(const std::string& name);

	// Debug another day
	
	//
}

