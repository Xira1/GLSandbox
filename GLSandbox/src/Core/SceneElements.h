#pragma once

#include <glm/glm.hpp>
#include "../Camera/Camera.h"
#include "../API/OpenGL/Types/GL_framebuffer.h"

struct SceneElements {
	SceneElements(glm::vec3& lp, glm::vec3& lc, glm::vec3& fc, glm::vec3& seed, glm::mat4& pm, Camera& c, FrameBufferObject& fbo) :
		lightPos(lp), lightColor(lc), fogColor(fc), projMatrix(pm), cam(c), sceneFBO(fbo), seed(seed) {
	};

	glm::vec3& lightPos, & lightColor, & fogColor, & seed;
	glm::mat4 projMatrix;
	Camera& cam;
	OpenGLFrameBuffer& sceneFBO;
	bool wireframe = false;
};