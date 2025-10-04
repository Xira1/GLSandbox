#include "GL_renderer.h"

#include "Types/GL_shader.h"
#include "Types/GL_framebuffer.h"
#include "Types/GL_texture.h"
#include "../AssetManagement/AssetManager.h"
#include "../Camera/Camera.h"
#include "../Types/Skybox.hpp"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"
#include "../World/WorldGrid.hpp"
#include "../Core/Scene.hpp"
#include "../../Util.hpp"

namespace OpenGLRenderer {
	OpenGLDetachedMesh cubeMeshPlayer;

	struct Shaders {
		Shader skybox;
		Shader solidColor;
		Shader textureShader;
		Shader gridShader;
	} g_shaders;

	struct Textures {
		OpenGLTexture cubeTexture;
	} g_textures;

	Skybox g_skybox;
	Grid g_grid;

	void RenderSkyBox();
	void RenderGrid();
	void RenderCubePlayer();
	void RenderScene();

	void Init() {
		std::vector<Vertex> cubeVertPlayer = Cube::GetVertices();
		std::vector<uint32_t> cubeIndPlayer = Cube::GetIndices();
		cubeMeshPlayer.UpdateVertexBuffer(cubeVertPlayer, cubeIndPlayer);

		LoadTextures();
		LoadShaders();

		g_grid.Init();
		g_skybox.Init();
	}

	void RenderFrame() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		RenderSkyBox();
		RenderGrid();
		RenderCubePlayer();
		RenderScene();

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void RenderScene() {
		for (RenderItem& renderItem : Scene::GetRenderItems()) {
			OpenGLDetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
			if (mesh) {
				g_shaders.textureShader.SetMat4("model", renderItem.modelMatrix);
				glBindVertexArray(mesh->GetVAO());
				glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
			}
		}
	}

	void RenderCubePlayer() {
		glEnable(GL_DEPTH_TEST);
		Transform player;
		player.position = Camera::GetPlayerPos();
		player.scale = glm::vec3(0.3f);

		g_shaders.solidColor.Use();
		g_shaders.solidColor.SetVec4("uniformColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		g_shaders.solidColor.SetMat4("view", Camera::GetViewMatrixPlayer());
		g_shaders.solidColor.SetMat4("projection", Camera::GetProjectionMatrix());
		g_shaders.solidColor.SetMat4("model", player.to_mat4());

		glBindVertexArray(cubeMeshPlayer.GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMeshPlayer.GetIndexCount(), GL_UNSIGNED_INT, 0);
	}

	void RenderSkyBox() {
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, g_skybox.cubemap.ID);

		g_shaders.skybox.Use();
		g_shaders.skybox.SetInt("skybox", 0);
		g_shaders.skybox.SetFloat("darknessFactor", 0.3f);
		g_shaders.skybox.SetMat4("view", Camera::GetViewMatrixPlayer());
		g_shaders.skybox.SetMat4("projection", Camera::GetProjectionMatrix());

		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(g_skybox.VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
	}

	void RenderGrid() {
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

	void LoadTextures() {
		std::cout << "\n";
		g_textures.cubeTexture.LoadFromFile("res/textures/wall.jpg");
		g_textures.cubeTexture.LoadFromFile("res/textures/rock_texture.png");
	}

	void LoadShaders() {
		if (g_shaders.solidColor.Load({ "GL_solid_color.vert", "GL_solid_color.frag" }) &&
			g_shaders.skybox.Load({ "GL_skybox.vert", "GL_skybox.frag" }) &&
			g_shaders.textureShader.Load({ "GL_texture_sample.vert", "GL_texture_sample.frag" }) &&
			g_shaders.gridShader.Load({ "GL_grid.vert", "GL_grid.frag" }) /**/) {
			std::cout << "Shaders load succesfully\n";
		}
	}
}