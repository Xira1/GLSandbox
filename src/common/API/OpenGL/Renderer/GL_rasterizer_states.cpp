#include "GL_renderer.h"


namespace OpenGLRenderer {
	void InitRasterizerState() {
		OpenGLRasterizerState* skybox = CreateRasterizerState("Skybox");
		skybox->blendEnabled = false;
		skybox->cullfaceEnabled = false;
		skybox->depthTestEnabled = false;
		skybox->depthMask = false;
		skybox->dethFunc = GL_LESS;

		OpenGLRasterizerState* grid = CreateRasterizerState("Grid");
		grid->blendEnabled = true;
		grid->cullfaceEnabled = false;
		grid->depthTestEnabled = false;
		grid->depthMask = false;
		grid->blendFuncSrcfactor = GL_SRC_ALPHA;
		grid->blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
	}
}