#include "GL_renderer.h"

#include "../Types/GL_texture.h"
#include "../Core/Scene.hpp"
#include "../../../Util.hpp"
#include "Enums.h"

namespace OpenGLRenderer {
	std::unordered_map<std::string, OpenGLShader> g_shaders;
	std::unordered_map<std::string, OpenGLFrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, OpenGLCubemapView> g_cubemapView;
	std::unordered_map<std::string, OpenGLTextureSet> g_textureSets;
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

			if (texHandles.size() == 6) {
				g_cubemapView["SkyboxNightSky"] = OpenGLCubemapView(texHandles);
			}
		}
	}

	void Init() {
		g_textureSets["CloudSet"] = OpenGLTextureSet(4, 1920, 1080);

		g_frameBuffers["GBuffer"] = OpenGLFrameBuffer("GBuffer", 1920, 1080);
		g_frameBuffers["GBuffer"].CreateAttachment("BaseColor", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateAttachment("Normal", GL_RGBA16F);
		g_frameBuffers["GBuffer"].CreateAttachment("RMA", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateAttachment("FinalLighting", GL_RGBA16F, GL_LINEAR, GL_LINEAR);
		g_frameBuffers["GBuffer"].CreateAttachment("WorldPosition", GL_RGBA32F);
		g_frameBuffers["GBuffer"].CreateAttachment("Emissive", GL_RGBA8);
		g_frameBuffers["GBuffer"].CreateDepthAttachment(GL_DEPTH_COMPONENT32F);

		g_frameBuffers["CloudBuffer"] = OpenGLFrameBuffer("CloudBuffer", 1920, 1080);
		g_frameBuffers["CloudBuffer"].CreateColorArray(2, GL_RGBA32F);

		g_frameBuffers["FinalImage"] = OpenGLFrameBuffer("FinalImage", 1920, 1080);
		g_frameBuffers["FinalImage"].CreateAttachment("Color", GL_RGBA16F);

		LoadShaders();

		InitClouds();
	}

	void LoadShaders() {
		g_shaders["SolidColor"] = OpenGLShader({ "GL_solid_color.vert", "GL_solid_color.frag" });
		g_shaders["Skybox"] = OpenGLShader({ "GL_skybox.vert", "GL_skybox.frag" });
		g_shaders["Grid"] = OpenGLShader({ "GL_grid.vert", "GL_grid.frag" });
		g_shaders["Light"] = OpenGLShader({ "GL_light.vert", "GL_light.frag" });

		g_shaders["VolumetricClouds"] = OpenGLShader({ "GL_volumetric_clouds.comp" });
		g_shaders["Perlinworley"] = OpenGLShader({ "GL_perlinworley.comp" });
		g_shaders["Worley"] = OpenGLShader({ "GL_worley.comp" });
		g_shaders["Weather"] = OpenGLShader({ "GL_weather.comp" });

		g_shaders["CloudsPostProcess"] = OpenGLShader({ "GL_screen.vert", "GL_clouds_post.frag" });
		g_shaders["PostProcessing"] = OpenGLShader({ "GL_screen.vert", "GL_post_processing.frag" });

		std::cout << "\n\nShaders load successfully\n";
	}
	
	void RenderFrame() {
		OpenGLFrameBuffer& gBuffer = g_frameBuffers["GBuffer"];
		OpenGLFrameBuffer& finalImageBuffer = g_frameBuffers["FinalImage"];

		glDisable(GL_DITHER);

		CloudsPass();

		ClearRenderTargets();

		SkyBoxPass();
		GridPass();
		RenderLightning();
		RenderCubePlayer();

		OpenGLRenderer::BlitFrameBuffer(&gBuffer, &finalImageBuffer, "FinalLighting", "Color", GL_COLOR_BUFFER_BIT, GL_LINEAR);
		OpenGLRenderer::BlitToDefaultFrameBuffer(&gBuffer, "BaseColor", GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void ClearRenderTargets() {
		OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

		glDepthMask(GL_TRUE);

		gBuffer->Bind();
		gBuffer->ClearAttachment("BaseColor", 0, 0, 0, 0);
		gBuffer->ClearAttachment("Normal", 0, 0, 0, 0);
		gBuffer->ClearAttachment("RMA", 0, 0, 0, 0);
		gBuffer->ClearAttachment("WorldPosition", 0 ,0);
		gBuffer->ClearAttachment("Emissive", 0, 0, 0, 0);
		gBuffer->ClearDepthAttachment();
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
		GameObject* Lattern = Scene::GetGameObjectByName("Lattern");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		shader->Use();
		shader->SetMat4("projection", Camera::GetInstance().GetProjectionMatrix());
		shader->SetMat4("view", Camera::GetInstance().GetViewMatrixPlayer());
		shader->SetMat4("model", glm::mat4(1));
		shader->SetVec3("viewPos", Camera::GetInstance().GetCameraPosition());
		shader->SetVec3("lightPos", Lattern->GetModelPosition() + glm::vec3(0.0f, 0.2f, 0.0f));
		shader->SetVec3("lightColor", glm::vec3(0.9f, 0.96f, 0.9f));
		shader->SetFloat("lightIntensity", 0.98f);

		RenderScene(*shader);
	}

	void RenderCubePlayer() {
		OpenGLShader* shader = GetShader("SolidColor");
		OpenGLDetachedMesh* cubeMeshPlayer = AssetManager::GetCubeMesh();

		glEnable(GL_DEPTH_TEST);
		if (Camera::GetInstance().GetCameraMode() == CameraMode::FIRST_PERSON) {
			return;
		}

		Transform player;
		player.position = Camera::GetInstance().GetPosition();
		player.scale = glm::vec3(0.2f);

		shader->Use();
		shader->SetVec3("uniformColor", glm::vec3(1.0f, 1.0f, 0.0f));
		shader->SetMat4("view", Camera::GetInstance().GetViewMatrixPlayer());
		shader->SetMat4("projection", Camera::GetInstance().GetProjectionMatrix());
		shader->SetMat4("model", player.to_mat4());

		glBindVertexArray(cubeMeshPlayer->GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMeshPlayer->GetIndexCount(), GL_UNSIGNED_INT, 0);
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

	OpenGLTextureSet* GetTextureSet(const std::string& name) {
		auto it = g_textureSets.find(name);
		return (it != g_textureSets.end()) ? &it->second : nullptr;
	}

	OpenGLRasterizerState* GetRasterizerState(const std::string& name) {
		auto it = g_rasterizerStates.find(name);
		return (it != g_rasterizerStates.end()) ? &it->second : nullptr;
	}

	OpenGLRasterizerState* CreateRasterizerState(const std::string& name) {
		g_rasterizerStates[name] = OpenGLRasterizerState();
		return &g_rasterizerStates[name];
	}

	/*void DrawQuad() {
		static OpenGLDetachedMesh* mesh = AssetManager::GetMeshByModelNameMeshName("Primitives", "Quad");
		glBindVertexArray(mesh->GetVAO());
		glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
	}*/

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