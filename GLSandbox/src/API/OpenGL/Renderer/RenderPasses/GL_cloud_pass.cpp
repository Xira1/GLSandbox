#include "../GL_renderer.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

namespace OpenGLRenderer {
	void InitClouds() {
		OpenGLShader* perlinShader = GetShader("Perlinworley");
		OpenGLTexture3D* perlinNoiseTex = GetTexture3D("PerlinNoise");;

		if (!perlinShader || !perlinNoiseTex) return;

		glBindImageTexture(0, perlinNoiseTex->GetHandle(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R16F);
		perlinShader->Use();
		perlinShader->SetVec3("u_resolution", glm::vec3(128, 128, 128));
		glActiveTexture(GL_TEXTURE0);
		perlinShader->SetInt("outVolTex", 0);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		glGenerateMipmap(GL_TEXTURE_3D);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	}

	void CloudsPass() {
		OpenGLShader* cloudShader = GetShader("VolumetricClouds");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
		OpenGLFrameBuffer* cloudsFBO = GetFrameBuffer("CloudsBuffer");
		OpenGLTexture3D* perlinNoiseTex = GetTexture3D("PerlinNoise");

		if (!cloudShader || !cloudsFBO || !perlinNoiseTex) return;

		glm::mat4 projection = Camera::GetInstance().GetProjectionMatrix();
		glm::mat4 view = Camera::GetInstance().GetViewMatrixPlayer();
		glm::mat4 invProj = glm::inverse(projection);
		glm::mat4 invView = glm::inverse(view);
		glm::mat4 invViewProj = glm::inverse(projection * view);
		glm::vec3 cameraPos = Camera::GetInstance().GetPosition();

		static float time = 0.0f;
		time += 1.0f / 60.0f;

		glm::vec3 lightDir = glm::normalize(glm::vec3(0.3f, 0.8f, 0.2f));

		cloudShader->Use();
		cloudShader->SetSampler3D("perlinTex", perlinNoiseTex->GetHandle(), 0);
		cloudShader->SetMat4("invViewProj", invViewProj);
		cloudShader->SetVec3("cameraPos", cameraPos);
		cloudShader->SetVec3("lightDir", lightDir);
		cloudShader->SetFloat("time", time);
		cloudShader->SetVec2("resolution", { cloudsFBO->GetWidth(), cloudsFBO->GetHeight() });
		
		glBindImageTexture(0, cloudsFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute((cloudsFBO->GetWidth() + 15) / 16, (cloudsFBO->GetHeight() + 15) / 16, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}