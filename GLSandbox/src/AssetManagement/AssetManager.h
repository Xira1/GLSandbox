#pragma once

#include "Common.h"
#include "../Types/Model.hpp"
#include "../Types/Texture.h"
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

	// Models
	void LoadModelFromData(Model& model, ModelData& modelData);
	int GetModelIndexByName(const std::string& name);
	Model* CreateModel(const std::string& name);
	Model* GetModelByIndex(int index);
	Model* GetModelByName(const std::string& name);

	// Material
	int GetMaterialIndex(const std::string& name);
	std::string& GetMaterialNameByIndex(int index);
	Material* GetMaterialByIndex(int index);
	Material* GetDefaultMaterial();

	// TODO: Mesh 
	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t> indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
	OpenGLDetachedMesh* GetMeshByIndex(int index);

	// Textures
	int GetTextureIndexByName(const std::string& name, bool warning = true);
	int GetTextureCount();
	Texture* GetTextureByIndex(int index);
	Texture* GetTextureByName(const std::string& name);
}