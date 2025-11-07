#include "Grid.h"

#include "../API/OpenGL/GL_backend.h"
#include "../Camera/Camera.h"

void Grid::Init() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindVertexArray(0);
}

void Grid::RenderGrid(OpenGLShader& shader, OpenGLFrameBuffer& fbo) {
	fbo.Bind();
	fbo.SetViewport();
	fbo.DrawBuffers({ "Color" });

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	shader.Use();

	glm::mat4 invViewProj = glm::inverse(Camera::GetProjectionMatrix() * Camera::GetViewMatrixPlayer());

	shader.SetMat4("uInverseViewProjection", invViewProj);
	shader.SetVec3("uCameraPosition", Camera::GetViewPos());
	shader.SetVec2("uResolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));
	shader.SetVec3("uGridColor", glm::vec3(0.5f));
	shader.SetFloat("uGridSize", 0.2f);
	shader.SetFloat("uMajorFactor", 10.0f);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}