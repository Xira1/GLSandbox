#include "GL_renderer.h"

#include "Types/GL_shader.h"
#include "Types/GL_framebuffer.h"
#include "../../Util.hpp"
#include "../Core/Camera.h"
#include "../Types/Skybox.hpp"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"

namespace OpenGLRenderer {
	OpenGLDetachedMesh cubeMesh;

	struct Shaders {
		Shader skybox;
		Shader solidColor;
	} g_shaders;

	struct Framebuffers {
		OpenGLFramebuffer main;
	} g_framebuffers;

	Skybox g_skybox;

	void RenderSkyBox();
	void RenderCamPos();
	void RenderCube();

	std::vector<Vertex> CreateCubeVertexData(float size) {							
		std::vector<glm::vec3> positions = Primitives::CreateCubeVertices(size);	
		std::vector<Vertex> vertices;												
																					
		for (const auto& pos : positions) {											
			Vertex v;																
			v.position = pos;														
			v.normal = glm::vec3(pos);												
			v.uv = glm::vec2(0.0f);													
			v.tangent = glm::vec3(1.0f, 0.0f, 0.0f); 								
			vertices.push_back(v);													
		}																			
		return vertices;															
	}

	void Init() {
		std::vector<Vertex> cubeVert = CreateCubeVertexData(0.5f);			       
		std::vector<unsigned int> cubeInd = Primitives::CreateCubeIndices();       
		cubeMesh.UpdateVertexBuffer(cubeVert, cubeInd);						       
		cubeMesh.SetName("cube");											       
																			 
		LoadShaders();

		g_skybox.Init();
	}

	void RenderFrame() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		RenderSkyBox();
		RenderCube();
		RenderCamPos();

		int width, height;
		glfwGetWindowSize(OpenGLBackend::GetWindowPtr(), &width, &height);
	

		glfwSwapBuffers(OpenGLBackend::GetWindowPtr());
		glfwPollEvents();
	}

	void RenderCube() {
		Transform c;
		c.rotation = glm::vec3(glfwGetTime() / PI);
		g_shaders.solidColor.Use();
		g_shaders.solidColor.SetMat4("view", Camera::GetViewMatrix());
		g_shaders.solidColor.SetMat4("projection", Camera::GetProjectionMatrix());
		g_shaders.solidColor.SetMat4("model", c.to_mat4());

		glBindVertexArray(cubeMesh.GetVAO());
		glDrawElements(GL_TRIANGLES, cubeMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
	}

	void RenderCamPos() {
		//std::string text = "Cam pos: " + Util::Vec3ToString(Camera::GetViewPos());
		std::string text = "Cam pos: " + Util::Vec3ToString(Camera::GetViewRotation());
		std::cout << text << "\n"; // change it to text on screen
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
		glBindVertexArray(g_skybox.vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDepthFunc(GL_LESS);
	}

	void LoadShaders() {
		if (g_shaders.solidColor.Load({ "gl_solid_color.vert", "gl_solid_color.frag" }) &&
			g_shaders.skybox.Load({ "GL_skybox.vert", "GL_skybox.frag" }) /**/) {
			std::cout << "Shaders load succesfully\n";
		}
	}
}