#include "../GL_renderer.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

namespace OpenGLRenderer {
	VolumetricCloudsCreateInfo createInfo;

	void InitClouds() {
		InitQuad();

		OpenGLShader* perlinShader = GetShader("Perlinworley");
		OpenGLShader* worleyShader = GetShader("Worley");
		OpenGLShader* weatherShader = GetShader("Weather");
		
		if (!perlinShader || !worleyShader || !weatherShader) {
			std::cout << "\n[Clouds system] One or more cloud shaders missing!\n";
			return;
		}

		createInfo.postProcess = true;

		if (createInfo.perlinTex == 0) {
			std::cout << "\n[Clouds system] Creating Perlin 3D texture...\n";

			glGenTextures(1, &createInfo.perlinTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, createInfo.perlinTex);

			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 128, 128, 128, 0, GL_RGBA, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_3D);
		}

		glBindImageTexture(0, createInfo.perlinTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		perlinShader->Use();
		perlinShader->SetVec3("u_resolution", glm::vec3(128.0f));
		perlinShader->SetInt("outVolTex", 0);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glGenerateMipmap(GL_TEXTURE_3D);

		if (createInfo.worley32 == 0) {
			std::cout << "[Clouds system] Creating Worley32 texture...\n";

			glGenTextures(1, &createInfo.worley32);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, createInfo.worley32);

			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 32, 32, 32, 0, GL_RGBA, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_3D);
		}

		glBindImageTexture(0, createInfo.worley32, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		worleyShader->Use();
		worleyShader->SetVec3("u_resolution", glm::vec3(32.0f));
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glGenerateMipmap(GL_TEXTURE_3D);

		if (createInfo.weatherTex == 0) {
			std::cout << "[Clouds system] Creating Weather 1024x1024 texture...\n";

			glGenTextures(1, &createInfo.weatherTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, createInfo.weatherTex);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		glBindImageTexture(0, createInfo.weatherTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

		weatherShader->Use();
		glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		std::cout << "[Clouds system] All cloud textures generated successfully!\n";
	}

    void CloudsPass() {
		OpenGLShader* cloudsShader = GetShader("VolumetricClouds");
		OpenGLShader* cloudsPPShader = GetShader("CloudsPostProcess");
		OpenGLTextureSet* cloudSet = GetTextureSet("CloudSet");
		OpenGLFrameBuffer* cloudBuffer = GetFrameBuffer("CloudBuffer");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

		if (!cloudsShader || !cloudSet || !cloudBuffer || !gBuffer) {
			return;
		}
		
		for (int i = 0; i < cloudSet->GetNTexturesCount(); ++i) {
			glBindImageTexture(i, cloudSet->GetColorAttachmentTextureByIndex(i), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		}

		cloudsShader->Use();
		cloudsShader->SetVec2("iResolution", glm::vec2(1920, 1080));
		cloudsShader->SetFloat("iTime", glfwGetTime());
		cloudsShader->SetMat4("inv_proj", glm::inverse(Camera::GetInstance().GetProjectionMatrix()));
		cloudsShader->SetMat4("inv_view", glm::inverse(Camera::GetInstance().GetViewMatrixPlayer()));
		cloudsShader->SetVec3("cameraPosition", Camera::GetInstance().GetCameraPosition());
		cloudsShader->SetFloat("FOV", 1);
		cloudsShader->SetVec3("lightDirection", glm::normalize(createInfo.lightPos));
		cloudsShader->SetVec3("lightColor", createInfo.lightColor);

		cloudsShader->SetFloat("coverage_multiplier", createInfo.coverage);
		cloudsShader->SetFloat("cloudSpeed", createInfo.cloudSpeed);
		cloudsShader->SetFloat("crispiness", createInfo.crispiness);
		cloudsShader->SetFloat("curliness", createInfo.curliness);
		cloudsShader->SetFloat("absorption", createInfo.absorption * 0.01);
		cloudsShader->SetFloat("densityFactor", createInfo.density);

		cloudsShader->SetVec3("cloudColorTop", createInfo.cloudColorTop);
		cloudsShader->SetVec3("cloudColorBottom", createInfo.cloudColorBottom);

		cloudsShader->SetVec3("skyColorTop", createInfo.skyColorTop);
		cloudsShader->SetVec3("skyColorBottom", createInfo.skyColorBottom);

		glm::mat4 vp = Camera::GetInstance().GetProjectionMatrix() * Camera::GetInstance().GetViewMatrixPlayer();
		cloudsShader->SetMat4("invViewProj", glm::inverse(vp));
		cloudsShader->SetMat4("gVP", vp);

		cloudsShader->SetSampler3D("cloud", createInfo.perlinTex, 0);
		cloudsShader->SetSampler3D("worley32", createInfo.worley32, 1);
		cloudsShader->SetSampler2D("weatherTex", createInfo.weatherTex, 2);
		cloudsShader->SetSampler2D("depthMap", gBuffer->GetDepthAttachmentHandle(), 3);

		glDispatchCompute(INT_CEIL(1920, 16), INT_CEIL(1080, 16), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (createInfo.postProcess && cloudsPPShader) {
			cloudBuffer->Bind();
			cloudBuffer->SetViewport();
			cloudBuffer->DrawBuffer("tex_0");

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			cloudsPPShader->Use();

			cloudsPPShader->SetSampler2D("clouds", cloudSet->GetColorAttachmentTextureByIndex(0), 0);
			cloudsPPShader->SetSampler2D("emissions", cloudSet->GetColorAttachmentTextureByIndex(1), 1);
			cloudsPPShader->SetSampler2D("depthMap", gBuffer->GetDepthAttachmentHandle(), 2);

			cloudsPPShader->SetVec2("cloudRenderResolution", glm::vec2(1920, 1080));
			cloudsPPShader->SetVec2("resolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));

			Transform lightModel;
			lightModel.position = createInfo.lightPos;
			glm::vec4 pos = vp * lightModel.to_mat4() * glm::vec4(0.0, 60.0, 0.0, 1.0);
			pos = pos / pos.w;
			pos = pos * 0.5f + 0.5f;

			cloudsPPShader->SetVec4("lightPos", pos);

			bool isLightInFront = false;
			float lightDotCameraFront = glm::dot(glm::normalize(createInfo.lightPos - Camera::GetInstance().GetCameraPosition()), glm::normalize(Camera::GetInstance().GetForward()));
			if (lightDotCameraFront > 0.2) {
				isLightInFront = true;
			}

			cloudsPPShader->SetBool("isLightInFront", isLightInFront);
			cloudsPPShader->SetBool("enableGodRays", createInfo.enableGodRays);
			cloudsPPShader->SetFloat("lightDotCameraFront", lightDotCameraFront);

			cloudsPPShader->SetFloat("time", glfwGetTime());

			DrawQuad();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}