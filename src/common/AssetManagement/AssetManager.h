#pragma once

#include "Common/Common.h"
#include "Renderer/Types/Model.hpp"
#include "Renderer/Types/Texture.h"
#include "Importer/AssimpImporter.h"
#include "Types/Animation/Animation.h"
#include "API/OpenGL/Types/GL_detachedMesh.hpp"

namespace AssetManager {
	void Init();
	void Update();

	// Models
	int GetModelIndexByName(const std::string& name);
	Model* CreateModel(const std::string& name);
	Model* GetModelByIndex(int index);
	Model* GetModelByName(const std::string& name);
	void BuildPrimitives();

	// Material
	int GetMaterialIndex(const std::string& name);
	std::string& GetMaterialNameByIndex(int index);
	Material* GetMaterialByIndex(int index);
	Material* GetDefaultMaterial();

	// Animations 
	std::vector<Animation>& GetAnimations();
	Animation* GetAnimationByName(const std::string& name);
	Animation* GetAnimationByIndex(int index);
	int GetAnimationIndexByName(const std::string& name);

	// Mesh 
	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	OpenGLDetachedMesh* GetMeshByIndex(int index);
	OpenGLDetachedMesh* GetCubeMesh();
	OpenGLDetachedMesh* GetMeshByModelNameMeshName(const std::string& modelName, const std::string& meshName);

	// Textures
	int GetTextureIndexByName(const std::string& name, bool warning = true);
	int GetTextureCount();
	Texture* GetTextureByIndex(int index);
	Texture* GetTextureByName(const std::string& name);

	// Loads
	void LoadModelsAsync();
	void LoadModelFromData(Model& model, ModelData& modelData);
	void LoadTextureMinimum();
	void LoadTexturesAsync();
	void LoadTexture(Texture* texture);
	void LoadPendingTexturesAsync();
	void LoadPendingAnimationsAsync();
	void LoadAnimations(Animation* animation);
}