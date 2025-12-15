#include "../GL_renderer.h"

namespace OpenGLRenderer {
	void GridPass() {
		OpenGLShader* shader = GetShader("Grid");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

		/*gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "BaseColor" });*/
		shader->Use();

		SetRasterizerState("Grid");

		glBindVertexArray(OpenGLBackend::GetVertexDataVAO());

		glDepthMask(GL_FALSE);

		glm::mat4 invViewProj = glm::inverse(Camera::GetInstance().GetProjectionMatrix() * Camera::GetInstance().GetViewMatrixPlayer());

		shader->SetMat4("uInverseViewProjection", invViewProj);
		shader->SetVec3("uCameraPosition", Camera::GetInstance().GetCameraPosition());
		shader->SetVec2("uResolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));
		shader->SetVec3("uGridColor", glm::vec3(0.5f));
		shader->SetFloat("uGridSize", 0.2f);
		shader->SetFloat("uMajorFactor", 10.0f);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDepthMask(GL_TRUE);
	}
}