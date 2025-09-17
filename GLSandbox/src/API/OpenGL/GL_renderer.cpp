#include "GL_renderer.h"

#include "Types/GL_shader.h"
#include "Types/GL_framebuffer.h"
#include "Types/GL_texture.h"
#include "../../Util.hpp"
#include "../Core/Camera.h"
#include "../Types/Skybox.hpp"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"
#include "../World/WorldGrid.hpp"

namespace OpenGLRenderer {
	OpenGLDetachedMesh cubeMesh;

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
	void RenderCube();
	void RenderGrid();

	void Init() {
		std::vector<Vertex> cubeVert = Cube::GetVertices();
		std::vector<uint32_t> cubeInd = Cube::GetIndices(); 
		cubeMesh.UpdateVertexBuffer(cubeVert, cubeInd);
		
		g_grid.Init();
		g_skybox.Init();

		LoadTextures();
		LoadShaders();
	}

	void RenderFrame() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		RenderSkyBox();
		RenderGrid();
		RenderCube();

		int width, height;
		glfwGetWindowSize(OpenGLBackend::GetWindowPtr(), &width, &height);

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void RenderCube() {
		glEnable(GL_DEPTH_TEST);

		g_shaders.textureShader.Use();
		g_textures.cubeTexture.Bind(0);
		g_shaders.textureShader.SetInt("textureSampler", 0);

		Transform c;
		c.position = glm::vec3(0.0f, 2.0f, 0.0f);
		c.rotation = glm::vec3(glfwGetTime() / PI);
		g_shaders.textureShader.SetMat4("view", Camera::GetViewMatrix());
		g_shaders.textureShader.SetMat4("projection", Camera::GetProjectionMatrix());
		g_shaders.textureShader.SetMat4("model", c.to_mat4());

		glBindVertexArray(cubeMesh.GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);

		glDisable(GL_DEPTH_TEST);
	}

	void RenderSkyBox() {
		glDisable(GL_DEPTH_TEST);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, g_skybox.cubemap.ID);

		g_shaders.skybox.Use();
		g_shaders.skybox.SetInt("skybox", 0);
		g_shaders.skybox.SetFloat("darknessFactor", 0.3f);
		g_shaders.skybox.SetMat4("view", Camera::GetViewMatrix());
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

		glm::mat4 invViewProj = glm::inverse(Camera::GetProjectionMatrix() * Camera::GetViewMatrix());

		g_shaders.gridShader.SetMat4("uInverseViewProjection", invViewProj);
		g_shaders.gridShader.SetVec3("uCameraPosition", Camera::GetViewPos());
		g_shaders.gridShader.SetVec2("uResolution", glm::vec2(OpenGLBackend::GetWindowWidth(), OpenGLBackend::GetWindowHeight()));
		g_shaders.gridShader.SetVec3("uGridColor", glm::vec3(0.6f));
		g_shaders.gridShader.SetFloat("uGridSize", 0.3f);
		g_shaders.gridShader.SetFloat("uMajorFactor", 10.0f);

		glBindVertexArray(g_grid.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	void LoadTextures() {
		g_textures.cubeTexture.LoadFromFile("res/textures/wall.jpg");
	}

	void LoadShaders() {
		if (g_shaders.solidColor.Load({ "GL_solid_color.vert", "GL_solid_color.frag" }) &&
			g_shaders.skybox.Load({ "GL_skybox.vert", "GL_skybox.frag" }) &&
			g_shaders.textureShader.Load({ "GL_texture_sample.vert", "GL_texture_sample.frag"}) &&
			g_shaders.gridShader.Load({ "GL_grid.vert", "GL_grid.frag" }) /**/) {
			std::cout << "Shaders load succesfully\n";
		}
	}
}