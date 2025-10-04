#pragma once

#include "Common.h"
#include "../Types/Model.hpp"
#include "../Importer/AssimpImporter.h"
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"

struct PendingModel {
	std::string filePath;
	std::promise<Model*> promise;
	std::function<void(Model*)> onLoaded;
};

namespace AssetManager {
	void Init();
	void Update();

	// Threads
	void ShutdownThread();

	// Models
	void LoadModelFromData(Model& model, ModelData& modelData);
	int GetModelIndexByName(const std::string& name);
	Model* CreateModel(const std::string& name);
	Model* GetModelByIndex(int index);
	Model* GetModelByName(const std::string& name);

	// TODO: Material

	// TODO: Mesh 
	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t> indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
	OpenGLDetachedMesh* GetMeshByIndex(int index);

	// TODO: Texture
}