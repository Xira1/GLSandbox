#pragma once

#include "Primitives.h"

#include <stb_image.h>

struct CubemapTexture {

	GLuint ID;

	void Create(std::vector<std::string>& textures) { 
		stbi_set_flip_vertically_on_load(true);
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		int width, height, nrChannels;
		for (unsigned int i = 0; i < textures.size(); i++) {
			unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				GLint format = GL_RGB;
				if (nrChannels == 4)
					format = GL_RGBA;
				if (nrChannels == 1)
					format = GL_RED;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else {
				std::cout << "Failed to load cubemap: " << textures[i] << "\n";
				stbi_image_free(data);
			}
		}
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
};

struct Skybox {

	GLuint VAO = 0;
	CubemapTexture cubemap;

	void Init() {
		std::vector<std::string> skyboxTextureFilePath;
		skyboxTextureFilePath.push_back("res/textures/skybox/right.png");
		skyboxTextureFilePath.push_back("res/textures/skybox/left.png");
		skyboxTextureFilePath.push_back("res/textures/skybox/bottom.png");
		skyboxTextureFilePath.push_back("res/textures/skybox/top.png");
		skyboxTextureFilePath.push_back("res/textures/skybox/front.png");
		skyboxTextureFilePath.push_back("res/textures/skybox/back.png");

		cubemap.Create(skyboxTextureFilePath);

		std::vector<glm::vec3> cubeVertices = Primitives::CreateCubeVertices(0.5f);
		std::vector<unsigned int> cubeIndices = Primitives::CreateCubeIndices();
		unsigned int cubeVbo, cubeEbo;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &cubeVbo);
		glGenBuffers(1, &cubeEbo);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(glm::vec3), &cubeVertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), &cubeIndices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};