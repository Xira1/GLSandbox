#pragma once

#include "Types.h"
#include "Model.hpp"

struct GameObject {
	std::string m_name;
	Model* m_model = nullptr;
	Transform m_transform;
	std::vector<int> m_meshMaterialIndices;

	void SetName(const std::string& name);
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetSize(glm::vec3 size);
	void SetModel(const std::string& name);
	void SetMeshMaterialByMeshName(std::string meshName, const char* materialName);
	void PrintMeshNames();
	void UpdateRenderItems();

	std::vector<RenderItem>& GetRendredItems();

private:
	std::vector<RenderItem> m_renderItems;
};

