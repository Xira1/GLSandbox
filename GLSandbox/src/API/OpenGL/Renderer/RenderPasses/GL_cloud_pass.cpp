#include "../GL_renderer.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

namespace OpenGLRenderer {
	void InitClouds() {
		OpenGLShader* perlinShader = GetShader("Perlinworley");
		OpenGLShader* worleyShader = GetShader("Worley");

		OpenGLTexture* perlinNoiseTex = GetTexture("PerlinNoise3D");
		OpenGLTexture* worleyNoiseTex = GetTexture("Wolrey3D");

		if (!perlinShader || !perlinNoiseTex || !worleyShader || !worleyNoiseTex) return;

		glBindImageTexture(0, perlinNoiseTex->GetHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R16F);
		perlinShader->Use();
		perlinShader->SetVec3("u_resolution", glm::vec3(128, 128, 128));
		glActiveTexture(GL_TEXTURE0);
		perlinShader->SetInt("outVolTex", 0);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		glGenerateMipmap(GL_TEXTURE_3D);

		glBindImageTexture(0, worleyNoiseTex->GetHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R16F);
		worleyShader->Use();
		worleyShader->SetVec3("u_resolution", glm::vec3(32, 32, 32));
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	void CloudsPass() {
		OpenGLShader* cloudShader = GetShader("VolumetricClouds");
		OpenGLShader* postShader = GetShader("CloudsPostProcess");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
		OpenGLFrameBuffer* cloudsFBO = GetFrameBuffer("CloudsMain");
		OpenGLFrameBuffer* cloudsPPFBO = GetFrameBuffer("CloudsPost");
		OpenGLTexture* perlinNoiseTex = GetTexture("PerlinNoise3D");
		OpenGLTexture* worleyNoiseTex = GetTexture("Wolrey3D");
		OpenGLTexture* weatherTex = GetTexture("WeatherMap");

		if (!cloudShader || !postShader || !cloudsFBO || !cloudsPPFBO ||
			!perlinNoiseTex || !worleyNoiseTex || !weatherTex) return;

		glm::mat4 projection = Camera::GetProjectionMatrix();
		glm::mat4 view = Camera::GetViewMatrixPlayer();
		glm::mat4 invProj = glm::inverse(projection);
		glm::mat4 invView = glm::inverse(view);
		glm::mat4 invViewProj = glm::inverse(projection * view);
		glm::vec3 cameraPos = Camera::GetViewPos();

		static float time = 0.0f;
		time += 1.0f / 60.0f;

		cloudShader->Use();
		cloudShader->SetMat4("inv_proj", invProj);
		cloudShader->SetMat4("inv_view", invView);
		cloudShader->SetMat4("invViewProj", invViewProj);
		cloudShader->SetVec3("cameraPosition", cameraPos);
		cloudShader->SetFloat("iTime", time);
		cloudShader->SetVec2("iResolution", glm::vec2(cloudsFBO->GetWidth(), cloudsFBO->GetHeight()));

		glBindImageTexture(0, cloudsFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, perlinNoiseTex->GetHandle());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, worleyNoiseTex->GetHandle());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, weatherTex->GetHandle());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthAttachmentHandle());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentSlotByName("BaseColor"));
	}
}