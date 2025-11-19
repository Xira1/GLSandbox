#include "../GL_renderer.h"

namespace OpenGLRenderer {
	void SkyBoxPass() {
		OpenGLShader* shader = GetShader("Skybox");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
		OpenGLCubemapView* skyboxCubemapView = GetCubemapView("SkyboxNightSky");
		OpenGLDetachedMesh* mesh = AssetManager::GetCubeMesh();
		
		gBuffer->Bind();
		gBuffer->DrawBuffers({ "FinalLighting", "WorldPosition" });
		shader->Use();

		SetRasterizerState("Skybox");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapView->GetHandle());
		glBindVertexArray(OpenGLBackend::GetVertexDataVAO());
		glDepthMask(GL_FALSE);

		Transform skyboxTransform;
		skyboxTransform.position = Camera::GetInstance().GetPosition();
		skyboxTransform.scale = glm::vec3(200.0f);

		shader->SetInt("skybox", 0);
		shader->SetMat4("u_modelMatrix", skyboxTransform.to_mat4());

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
	}
}