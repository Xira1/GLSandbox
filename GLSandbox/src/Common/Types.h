#pragma once
#include "Common.h"
#include <stb_image.h>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

struct Vertex {
	Vertex() = default;
	Vertex(glm::vec3 pos) {
		position = pos;
	}

	Vertex(glm::vec3 pos, glm::vec3 norm) {
		position = pos;
		normal = norm;
	}

	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord) {
		position = pos;
		normal = norm;
		uv = texCoord;
	}

	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord, glm::vec3 _tangent) {
		position = pos;
		normal = norm;
		uv = texCoord;
		tangent = _tangent;
	}

	glm::vec3 position = glm::vec3(0);
	glm::vec3 normal = glm::vec3(0);
	glm::vec2 uv = glm::vec2(0);
	glm::vec3 tangent = glm::vec3(0);
};

struct Transform {
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	glm::mat4 to_mat4() {
		glm::mat4 m = glm::translate(glm::mat4(1), position);
		m *= glm::mat4_cast(glm::quat(rotation));
		m = glm::scale(m, scale);

		return m;
	}
};

struct RenderItem {
	glm::mat4 modelMatrix;
	int meshIndex;
	int baseColorTextureIndex;
	int normalColorTextureIndex;
	int rmaColorTextureIndex;
};

struct FileInfo {
	std::string path;
	std::string name;
	std::string ext;
	std::string dir;
	std::string GetFileNameWithExtension() {
		if (ext.length() > 0) {
			return name + "." + ext;
		}
		else {
			return name;
		}
	}
};