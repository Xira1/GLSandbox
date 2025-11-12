#include "GL_renderer.h"

#include "../Types/GL_texture.h"
#include "../Core/Scene.hpp"
#include "../../../Util.hpp"

namespace OpenGLRenderer {
	std::unordered_map<std::string, OpenGLShader> g_shaders;
	std::unordered_map<std::string, OpenGLFrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, OpenGLCubemapView> g_cubemapView;
	std::unordered_map<std::string, OpenGLTexture> g_textures;
	std::unordered_map<std::string, OpenGLRasterizerState> g_rasterizerStates;

	void RenderScene(OpenGLShader& shader);
	void RenderCubePlayer();
	void RenderLightning();

	void InitMain() {
		InitRasterizerState();

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
			if (!texture) continue;
			texHandles.push_back(texture->GetGLTexture().GetHandle());
		}
		if (texHandles.size() == 6) {
			g_cubemapView["SkyboxNightSky"] = OpenGLCubemapView(texHandles);
		}
	}

	void Init() {
		g_textures["PerlinNoise3D"] = OpenGLTexture();
		g_textures["PerlinNoise3D"].Texture3D(128, 128, 128);

		g_textures["Wolrey3D"] = OpenGLTexture();
		g_textures["Worley3D"].Texture3D(32, 32, 32);

		g_textures["WeatherMap"] = OpenGLTexture();
		g_textures["WeatherMap"].Texture2D(1024, 1024);

		g_frameBuffers["GBuffer"] = OpenGLFrameBuffer("Main", 1920, 1080);
		g_frameBuffers["GBuffer"].CreateAttachment("BaseColor", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateAttachment("Normal", GL_RGBA16F);
		g_frameBuffers["GBuffer"].CreateAttachment("RMA", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateAttachment("FinalLighting", GL_RGBA16F, GL_LINEAR, GL_LINEAR);
		g_frameBuffers["GBuffer"].CreateAttachment("WorldPosition", GL_RGBA32F);
		g_frameBuffers["GBuffer"].CreateAttachment("Emissive", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

		g_frameBuffers["CloudsMain"] = OpenGLFrameBuffer("CloudsMain", 1920, 1080);
		g_frameBuffers["CloudsMain"].CreateAttachment("Color", GL_RGBA16F);
		g_frameBuffers["CloudsMain"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

		g_frameBuffers["CloudsPost"] = OpenGLFrameBuffer("CloudsPostProcess", 1920, 1080);
		g_frameBuffers["CloudsPost"].CreateAttachment("Color", GL_RGBA16F);

		g_frameBuffers["FinalImage"] = OpenGLFrameBuffer("FinalImage", 1920, 1080);
		g_frameBuffers["FinalImage"].CreateAttachment("Color", GL_RGBA16F);

		LoadShaders();
		
		InitClouds();
	}

	void RenderFrame() {
		OpenGLFrameBuffer& gBuffer = g_frameBuffers["GBuffer"];
		OpenGLFrameBuffer& finalImageBuffer = g_frameBuffers["FinalImage"];

		gBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SkyBoxPass();
		GridPass();
		CloudsPass();

		RenderLightning();
		RenderCubePlayer();

		int width, height;
		glfwGetWindowSize(OpenGLBackend::GetWindowPtr(), &width, &height);

		gBuffer.BlitToDefaultFrameBuffer(&gBuffer, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

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
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
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

		g_shaders["VolumetricClouds"] = OpenGLShader({ "Clouds/GL_volumetric_clouds.comp" });
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

	OpenGLTexture* GetTexture(const std::string& name) {
		auto it = g_textures.find(name);
		return (it != g_textures.end()) ? &it->second : nullptr;
	}

	OpenGLRasterizerState* GetRasterizerState(const std::string& name) {
		auto it = g_rasterizerStates.find(name);
		return (it != g_rasterizerStates.end()) ? &it->second : nullptr;
	}

	OpenGLRasterizerState* CreateRasterizerState(const std::string& name) {
		g_rasterizerStates[name] = OpenGLRasterizerState();
		return &g_rasterizerStates[name];
	}

	void SetRasterizerState(const std::string& name) {
		OpenGLRasterizerState* rasterizerState = GetRasterizerState(name);
		if (!rasterizerState) {
			std::cout << "OpenGLRenderer::SetRasterizerState(const std::string& name) failed, because " << name << " does not exist\n";
			return;
		}

		rasterizerState->blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		rasterizerState->cullfaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		rasterizerState->depthMask ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
		rasterizerState->depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		if (rasterizerState->blendEnabled) {
			glBlendFunc(rasterizerState->blendFuncSrcfactor, rasterizerState->blendFuncDstfactor);
		}
		if (rasterizerState->depthTestEnabled) {
			glDepthFunc(rasterizerState->dethFunc);
		}
		if (rasterizerState->pointSize > 1.0f) {
			glPointSize(rasterizerState->pointSize);
		}
	}
}