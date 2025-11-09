#include "GL_renderer.h"

#include "../Types/GL_texture.h"
#include "../World/Skybox.h"
#include "../World/VolumetricClouds/VolumetricClouds.h"
#include "../World/Grid.h"
#include "../Core/Scene.hpp"
#include "../../../Util.hpp"

namespace OpenGLRenderer {
	std::unordered_map<std::string, OpenGLShader> g_shaders;
	std::unordered_map<std::string, OpenGLFrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, OpenGLCubemapView> g_cubemapView;

	void RenderScene(OpenGLShader& shader);
	void RenderCubePlayer();
	void RenderLightning();

	void InitMain() {
		// Skybox
		std::vector<Texture*> textures = {
			AssetManager::GetTextureByName("right"),
			AssetManager::GetTextureByName("left"),
			AssetManager::GetTextureByName("bottom"),
			AssetManager::GetTextureByName("top"),
			AssetManager::GetTextureByName("front"),
			AssetManager::GetTextureByName("back"),
		};

		std::vector<GLuint> texHandles;
		for (Texture* texture : textures) {
			if (!texture) return;
			texHandles.push_back(texture->GetGLTexture().GetHandle());
		}
		if (texHandles.size() == 6) {
			g_cubemapView["SkyboxNightSky"] = OpenGLCubemapView(texHandles);
		}
	}

	void Init() {
		g_frameBuffers["Main"] = OpenGLFrameBuffer("Main", 1920, 1080);
		g_frameBuffers["Main"].CreateAttachment("Color", GL_RGBA8);
		g_frameBuffers["Main"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

		LoadShaders();
	}

	void RenderFrame() {
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("Main");

		gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "Color" });
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SkyBoxPass();
		GridPass();
		CloudsPass();

		RenderLightning();
		RenderCubePlayer();

		int width, height;
		glfwGetWindowSize(OpenGLBackend::GetWindowPtr(), &width, &height);

		gBuffer->BlitToDefaultFrameBuffer("Color", 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void RenderScene(OpenGLShader& shader) {
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
		OpenGLShader* shader = GetShader("Light");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("Main");

		gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "Color" });

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		GameObject* Lattern = Scene::GetGameObjectByName("Lattern");

		shader->Use();
		shader->SetMat4("projection", Camera::GetProjectionMatrix());
		shader->SetMat4("view", Camera::GetViewMatrixPlayer());
		shader->SetMat4("model", glm::mat4(1));
		shader->SetVec3("viewPos", Camera::GetViewPos());
		shader->SetVec3("lightPos", Lattern->GetModelPosition() + glm::vec3(0.0f, 0.2f, 0.0f));
		shader->SetVec3("lightColor", glm::vec3(1.0f, 0.96f, 0.9f));
		shader->SetFloat("lightIntensity", 1.0f);
		RenderScene(*shader);
	}

	void RenderCubePlayer() {
		OpenGLShader* shader = GetShader("SolidColor");
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("Main");
		OpenGLDetachedMesh* cubeMeshPlayer = AssetManager::GetCubeMesh();

		gBuffer->Bind();
		gBuffer->SetViewport();
		gBuffer->DrawBuffers({ "Color" });

		glEnable(GL_DEPTH_TEST);
		if (Camera::GetCameraMode() == Camera::CameraMode::FIRST_PERSON) {
			return;
		}

		Transform player;
		player.position = Camera::GetPlayerPos();
		player.scale = glm::vec3(0.2f);

		shader->Use();
		shader->SetVec3("uniformColor", glm::vec3(1.0f, 1.0f, 0.0f));
		shader->SetMat4("view", Camera::GetViewMatrixPlayer());
		shader->SetMat4("projection", Camera::GetProjectionMatrix());
		shader->SetMat4("model", player.to_mat4());

		glBindVertexArray(cubeMeshPlayer->GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMeshPlayer->GetIndexCount(), GL_UNSIGNED_INT, 0);
	}


	void LoadShaders() {
		g_shaders["SolidColor"] = OpenGLShader({ "GL_solid_color.vert", "GL_solid_color.frag" });
		g_shaders["Skybox"] = OpenGLShader({ "GL_skybox.vert", "GL_skybox.frag" });
		g_shaders["Grid"] = OpenGLShader({ "GL_grid.vert", "GL_grid.frag" });
		g_shaders["Light"] = OpenGLShader({ "GL_light.vert", "GL_light.frag" });

		g_shaders["Clouds"] = OpenGLShader({ "Clouds/GL_volumetric_clouds.comp" });
		g_shaders["Perlinworley"] = OpenGLShader({ "Clouds/perlinworley.comp" });
		g_shaders["Worley"] = OpenGLShader({ "Clouds/worley.comp" });
		g_shaders["Weather"] = OpenGLShader({ "Clouds/GL_weather.comp" });
		g_shaders["CloudsPostProcess"] = OpenGLShader({ "Clouds/GL_clouds_post.frag" });

		g_shaders["PostProcessing"] = OpenGLShader({ "GL_post_processing.frag" });

		std::cout << "\nShaders load successfully\n";
	}

	OpenGLCubemapView* GetCubemapView(const std::string& name) {
		auto it = g_cubemapView.find(name);
		return (it != g_cubemapView.end()) ? &it->second : nullptr;
	}

	OpenGLFrameBuffer* GetFrameBuffer(const std::string& name) {
		auto it = g_frameBuffers.find(name);
		return (it != g_frameBuffers.end()) ? &it->second : nullptr;
	}

	OpenGLShader* GetShader(const std::string& name) {
		auto it = g_shaders.find(name);
		return (it != g_shaders.end()) ? &it->second : nullptr;
	}
}