#pragma once

#include "../API/OpenGL/Types/GL_framebuffer.hpp"
#include "../API/OpenGL/Types/GL_shader.h"
#include "../Types/Texture.h"


struct VolumetricClouds {
public:
	void Init();
	void RenderClouds();

	unsigned int GetCloudsTexture(int i);

private:
	int SRC_WIDTH, SRC_HEIGHT;
	static float coverage, cloudSpeed, crispiness, curliness, density, absorption;
	bool postProcess;

	static glm::vec3 cloudTopColor, cloudBottomColor;

	OpenGLShader* volumetricCloudsShader, *weatherShader, *postProcessCloudsShader, *postProcessShader;
	OpenGLFrameBuffer* cloudsFBO, *postProcessCloudsFBO;
	Texture tex3D;

	static unsigned int perlinTex, worley32, weatherTex;
};