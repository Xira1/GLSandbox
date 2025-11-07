#include "VolumetricClouds.h"

#include "../Camera/Camera.h"
#include "../Types/GameObject.h"
#include "../Core/Scene.hpp"
#include "../API/OpenGL/GL_backend.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

float VolumetricClouds::cloudSpeed = 450.0;
float VolumetricClouds::coverage = 0.45;
float VolumetricClouds::crispiness = 40.;
float VolumetricClouds::curliness = .1;
float VolumetricClouds::density = 0.02;
float VolumetricClouds::absorption = 0.35;

glm::vec3 VolumetricClouds::cloudTopColor = (glm::vec3(169., 149., 149.) * (1.5f / 255.f));
glm::vec3 VolumetricClouds::cloudBottomColor = (glm::vec3(65., 70., 80.) * (1.5f / 255.f));

unsigned int VolumetricClouds::worley32 = 0;
unsigned int VolumetricClouds::perlinTex = 0;
unsigned int VolumetricClouds::weatherTex = 0;

GLuint quadVAO;

void InitFullscreenQuad() {
	static GLuint quadVBO;

	if (quadVBO != 0) {
		return;
	}

	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

void VolumetricClouds::Init() {
	InitFullscreenQuad();

	volumetricCloudsShader = new OpenGLShader({ "Clouds/GL_volumetric_clouds.comp" });
	postProcessShader = new OpenGLShader({ "GL_post_process.frag" });
	postProcessCloudsShader = new OpenGLShader({ "Clouds/GL_clouds_post.frag" });

	cloudsFBO = new OpenGLFrameBuffer("cloudMain", 1920, 1080);
	cloudsFBO->CreateAttachment("cloudsColor", GL_RGBA16F);
	cloudsFBO->CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

	postProcessCloudsFBO = new OpenGLFrameBuffer("cloudPostFX", 1920, 1080);
	postProcessCloudsFBO->CreateAttachment("cloudsPP", GL_RGBA16F);

	if (!perlinTex) {
		OpenGLShader perlin;
		perlin.Load({ "Clouds/perlinworley.comp" });

		this->perlinTex = tex3D.GetGLTexture().Texture3D(128, 128, 128);
		perlin.Use();
		perlin.SetVec3("u_resolution", glm::vec3(128, 128, 128));
		glActiveTexture(GL_TEXTURE0);
		perlin.SetInt("outVolTex", 0);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worley32) {
		OpenGLShader worley;
		worley.Load({ "Clouds/worley.comp" });

		this->worley32 = tex3D.GetGLTexture().Texture3D(32, 32, 32);
		worley.Use();
		worley.SetVec3("u_resolution", glm::vec3(32, 32, 32));
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	// Compute weather
	weatherShader = new OpenGLShader({ "Clouds/GL_weather.comp" });

	if (!weatherTex) {
		this->weatherTex = tex3D.GetGLTexture().Texture2D(1024, 1024);

		glBindImageTexture(0, weatherTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		weatherShader->Use();
		glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}

void VolumetricClouds::RenderClouds() {
	for (int i = 0; i < cloudsFBO->GetColorAttachmentCount(); ++i) {
		GLuint tex = cloudsFBO->GetColorAttachmentByIndex(i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	GameObject* Lattern = Scene::GetGameObjectByName("Lattern");
	glm::vec3 lightDir = glm::normalize((Lattern->GetModelPosition() + glm::vec3(0.0f, 0.2f, 0.0f)) - Camera::GetViewPos());

	OpenGLShader& sh = *volumetricCloudsShader;
	sh.Use();
	sh.SetVec2("iResolution", glm::vec2(SRC_WIDTH, SRC_HEIGHT));
	sh.SetFloat("iTime", glfwGetTime());
	sh.SetMat4("inv_proj", glm::inverse(Camera::GetProjectionMatrix()));
	sh.SetMat4("inv_view", Camera::GetViewMatrixPlayer());
	sh.SetVec3("cameraPosition", Camera::GetPlayerPos());
	sh.SetVec3("lightDirection", lightDir);
	sh.SetVec3("lightColor", glm::vec3(1.0f, 0.96f, 0.9f));

	sh.SetFloat("coverage_multiplier", coverage);
	sh.SetFloat("cloudSpeed", cloudSpeed);
	sh.SetFloat("crispiness", crispiness);
	sh.SetFloat("curliness", curliness);
	sh.SetFloat("absorption", absorption * 0.01);
	sh.SetFloat("densityFactor", density);

	sh.SetVec3("cloudColorTop", cloudTopColor);
	sh.SetVec3("cloudColorBottom", cloudBottomColor);

	glm::mat4 invViewProj = Camera::GetProjectionMatrix() * Camera::GetViewMatrixPlayer();
	sh.SetMat4("invViewProj", glm::inverse(invViewProj));
	sh.SetMat4("gVP", invViewProj);

	sh.SetSampler3D("cloud", this->perlinTex, 0);
	sh.SetSampler3D("worley32", this->worley32, 1);
	sh.SetSampler2D("weatherTex", this->weatherTex, 2);
	sh.SetSampler2D("depthMap", cloudsFBO->GetDepthAttachmentHandle(), 3);

	OpenGLFrameBuffer& cloudMain = *cloudsFBO;
	GLuint cloudTex = cloudMain.GetColorAttachmentHandleByName("cloudsColor");
	glBindImageTexture(0, cloudTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(INT_CEIL(SRC_WIDTH, 16), INT_CEIL(SRC_HEIGHT, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Post process
	if (postProcess) {
		postProcessCloudsFBO->Bind();
		postProcessCloudsFBO->SetViewport();

		OpenGLShader& ppSh = *postProcessCloudsShader;
		ppSh.Use();

		ppSh.SetSampler2D("clouds", cloudsFBO->GetColorAttachmentByIndex(0), 0);
		ppSh.SetSampler2D("emissions", cloudsFBO->GetColorAttachmentByIndex(1), 1);
		ppSh.SetSampler2D("depthMap", cloudsFBO->GetDepthAttachmentHandle(), 2);

		ppSh.SetVec2("cloudRenderResolution", glm::vec2(SRC_WIDTH, SRC_HEIGHT));
		ppSh.SetVec2("resolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));

		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, Lattern->GetModelPosition());
		glm::vec4 pos = invViewProj * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;

		ppSh.SetVec4("lightPos", pos);
		ppSh.SetFloat("time", glfwGetTime());

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

unsigned int VolumetricClouds::GetCloudsTexture(int i) {
	return cloudsFBO->GetColorAttachmentByIndex(i);
}
