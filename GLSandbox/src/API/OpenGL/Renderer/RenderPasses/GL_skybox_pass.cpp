#include "../GL_renderer.h"

namespace OpenGLRenderer {
	void SkyBoxPass() {
		OpenGLShader* shader = GetShader("Skybox");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("Main");
		OpenGLCubemapView* skyboxCubemapView = GetCubemapView("SkyboxNightSky");
		OpenGLDetachedMesh* mesh = AssetManager::GetCubeMesh();
		
		gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "Color" });
		shader->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapView->GetHandle());
		glBindVertexArray(mesh->GetVAO());
		glDepthMask(GL_FALSE);

		shader->SetInt("skybox", 0);
		shader->SetFloat("darknessFactor", 0.5f);
		shader->SetMat4("view", glm::mat4(glm::mat3(Camera::GetViewMatrixPlayer())));
		shader->SetMat4("projection", Camera::GetProjectionMatrix());

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
	}
}