#include "GL_renderer.h"

#include "Types/GL_shader.h"
#include "Types/GL_texture.h"
#include "../AssetManagement/AssetManager.h"
#include "../Camera/Camera.h"
#include "../Types/Skybox.hpp"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"
#include "../API/OpenGL/Types/GL_framebuffer.hpp"
#include "../World/WorldGrid.hpp"
#include "../Core/Scene.hpp"
#include "../../Util.hpp"

namespace OpenGLRenderer {
	OpenGLDetachedMesh cubeMeshPlayer;

	struct Shaders {
		Shader skybox;
		Shader solidColor;
		Shader gridShader;
		Shader light;
	} g_shaders;

	struct FrameBuffers {
		OpenGLFrameBuffer main;
	} g_frameBuffers;

	Skybox g_skybox;
	Grid g_grid;

	void RenderSkyBox();
	void RenderGrid();
	void RenderCubePlayer();
	void RenderScene(Shader& shader);
	void RenderLightning();

	void Init() {
		g_frameBuffers.main.Create("Main", 1920, 1080);
		g_frameBuffers.main.CreateAttachment("Color", GL_RGBA8);
		g_frameBuffers.main.CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

		std::vector<Vertex> cubeVertPlayer = Cube::GetVertices();
		std::vector<uint32_t> cubeIndPlayer = Cube::GetIndices();
		cubeMeshPlayer.UpdateVertexBuffer(cubeVertPlayer, cubeIndPlayer);

		g_grid.Init();
		g_skybox.Init();

		LoadShaders();
	}

	void RenderFrame() {
		g_frameBuffers.main.Bind();
		g_frameBuffers.main.SetViewport();
		g_frameBuffers.main.DrawBuffers({ "Color" });
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderSkyBox();
		RenderGrid();
		RenderLightning();
		RenderCubePlayer();

		int width, height;
		glfwGetWindowSize(OpenGLBackend::GetWindowPtr(), &width, &height);


		g_frameBuffers.main.BlitToDefaultFrameBuffer("Color", 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void RenderScene(Shader& shader) {
		for (RenderItem& renderItem : Scene::GetRenderItems()) {
			OpenGLDetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
			if (mesh) {
				shader.SetMat4("model", renderItem.modelMatrix);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalColorTextureIndex)->GetGLTexture().GetHandle());
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaColorTextureIndex)->GetGLTexture().GetHandle());
				glBindVertexArray(mesh->GetVAO());
				glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
			}
		}
	}

	void RenderLightning() {
		g_frameBuffers.main.Bind();
		g_frameBuffers.main.SetViewport();
		g_frameBuffers.main.DrawBuffers({ "Color" });

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		GameObject* Lattern = Scene::GetGameObjectByName("Lattern");

		g_shaders.light.Use();
		g_shaders.light.SetMat4("projection", Camera::GetProjectionMatrix());
		g_shaders.light.SetMat4("view", Camera::GetViewMatrixPlayer());
		g_shaders.light.SetMat4("model", glm::mat4(1));
		g_shaders.light.SetVec3("viewPos", Camera::GetViewPos());
		g_shaders.light.SetVec3("lightPos", Lattern->GetModelPosition() + glm::vec3(0.0f, 0.2f, 0.0f));
		g_shaders.light.SetVec3("lightColor", glm::vec3(1.0f, 0.96f, 0.9f));
		g_shaders.light.SetFloat("lightIntensity", 1.0f);
		RenderScene(g_shaders.light);
	}

	void RenderCubePlayer() {
		g_frameBuffers.main.Bind();
		g_frameBuffers.main.SetViewport();
		g_frameBuffers.main.DrawBuffers({ "Color" });

		glEnable(GL_DEPTH_TEST);
		if (Camera::GetCameraMode() == Camera::CameraMode::FIRST_PERSON) {
			return;
		}

		Transform player;
		player.position = Camera::GetPlayerPos();
		player.scale = glm::vec3(0.2f);

		g_shaders.solidColor.Use();
		g_shaders.solidColor.SetVec3("uniformColor", glm::vec3(1.0f, 1.0f, 0.0f));
		g_shaders.solidColor.SetMat4("view", Camera::GetViewMatrixPlayer());
		g_shaders.solidColor.SetMat4("projection", Camera::GetProjectionMatrix());
		g_shaders.solidColor.SetMat4("model", player.to_mat4());

		glBindVertexArray(cubeMeshPlayer.GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMeshPlayer.GetIndexCount(), GL_UNSIGNED_INT, 0);
	}

	void RenderSkyBox() {
		g_frameBuffers.main.Bind();
		g_frameBuffers.main.SetViewport();
		g_frameBuffers.main.DrawBuffers({ "Color" });

		glEnable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, g_skybox.cubemap.ID);

		g_shaders.skybox.Use();
		g_shaders.skybox.SetInt("skybox", 0);
		g_shaders.skybox.SetFloat("darknessFactor", 1.0f);
		g_shaders.skybox.SetMat4("view", glm::mat4(glm::mat3(Camera::GetViewMatrixPlayer())));
		g_shaders.skybox.SetMat4("projection", Camera::GetProjectionMatrix());

		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(g_skybox.VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDepthFunc(GL_LESS);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void RenderGrid() {
		g_frameBuffers.main.Bind();
		g_frameBuffers.main.SetViewport();
		g_frameBuffers.main.DrawBuffers({ "Color" });

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		g_shaders.gridShader.Use();

		glm::mat4 invViewProj = glm::inverse(Camera::GetProjectionMatrix() * Camera::GetViewMatrixPlayer());

		g_shaders.gridShader.SetMat4("uInverseViewProjection", invViewProj);
		g_shaders.gridShader.SetVec3("uCameraPosition", Camera::GetViewPos());
		g_shaders.gridShader.SetVec2("uResolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));
		g_shaders.gridShader.SetVec3("uGridColor", glm::vec3(0.5f));
		g_shaders.gridShader.SetFloat("uGridSize", 0.2f);
		g_shaders.gridShader.SetFloat("uMajorFactor", 10.0f);

		glBindVertexArray(g_grid.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}


	void LoadShaders() {
		if (g_shaders.solidColor.Load({ "GL_solid_color.vert", "GL_solid_color.frag" }) &&
			g_shaders.skybox.Load({ "GL_skybox.vert", "GL_skybox.frag" }) &&
			g_shaders.gridShader.Load({ "GL_grid.vert", "GL_grid.frag" }) &&
			g_shaders.light.Load({ "GL_light.vert", "GL_light.frag" }) /**/) {
			std::cout << "\nShaders load successfully\n";
		}
	}
}