#include "../GL_renderer.h"

namespace OpenGLRenderer {
	void GridPass() {
		OpenGLShader* shader = GetShader("Grid");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
		OpenGLDetachedMesh* mesh = AssetManager::GetCubeMesh();

		gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "Color" });
		shader->Use();

		SetRasterizerState("Grid");

		glBindVertexArray(mesh->GetVAO());

		glDepthMask(GL_FALSE);

		glm::mat4 invViewProj = glm::inverse(Camera::GetProjectionMatrix() * Camera::GetViewMatrixPlayer());

		shader->SetMat4("uInverseViewProjection", invViewProj);
		shader->SetVec3("uCameraPosition", Camera::GetViewPos());
		shader->SetVec2("uResolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));
		shader->SetVec3("uGridColor", glm::vec3(0.5f));
		shader->SetFloat("uGridSize", 0.2f);
		shader->SetFloat("uMajorFactor", 10.0f);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
	}
}