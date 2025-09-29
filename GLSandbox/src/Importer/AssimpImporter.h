#pragma once

#include "Types.h"

struct MeshData {
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	int vertexCount;
	int indicesCount;
	glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::min());
	glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
};

struct ModelData {
	std::string name;
	uint32_t meshCount;
	//uint64_t timeStamp; // No need right now
	std::vector<MeshData> meshes;
	glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::min());
	glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
};

namespace AssimpImporter {
	ModelData ImportModel(const std::string filePath);
}