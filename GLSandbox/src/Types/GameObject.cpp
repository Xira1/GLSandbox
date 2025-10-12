#include "GameObject.h"

#include "../AssetManagement/AssetManager.h"

void GameObject::SetName(const std::string& name) {
	m_name = name;
}

void GameObject::SetPosition(glm::vec3 position) {
	m_transform.position = position;
}

void GameObject::SetRotation(glm::vec3 rotation) {
	m_transform.rotation = rotation;
}

void GameObject::SetSize(glm::vec3 size) {
	m_transform.scale = size;
}

void GameObject::SetModel(const std::string& name) {
	m_model = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName(name.c_str()));

	if (m_model) {
		m_meshMaterialIndices.resize(m_model->GetMeshCount());
		std::fill(m_meshMaterialIndices.begin(), m_meshMaterialIndices.end(), -1);
	}
	else {
		std::cout << "Failed to set model '" << name << "', it does not exist.\n";
	}

}

void GameObject::SetMeshMaterialByMeshName(std::string meshName, const char* materialName) {
	int materialIndex = AssetManager::GetMaterialIndex(materialName);
	if (m_model && materialIndex != -1) {
		for (int i = 0; i < m_model->GetMeshCount(); i++) {
			if (AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i])->GetName() == meshName) {
				m_meshMaterialIndices[i] = materialIndex;
				return;
			}
		}
	}
	if (!m_model) {
		std::cout << "Tried to call SetMeshMaterialByMeshName() but this GameObject has a nullptr m_model\n";
	}
	else if (materialIndex == -1) {
		std::cout << "Tried to call SetMeshMaterialByMeshName() but the material index was -1\n";
	}
	else {
		std::cout << "Tried to call SetMeshMaterialByMeshName() but the meshName '" << meshName << "' not found\n";
	}
}

void GameObject::UpdateRenderItems() {
	m_renderItems.clear();

	if (m_model) {
		for (int i = 0; i < m_model->GetMeshCount(); i++) {
			OpenGLDetachedMesh* mesh = AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i]);
			if (mesh) {
				RenderItem renderItem;
				renderItem.modelMatrix = m_transform.to_mat4();
				renderItem.meshIndex = m_model->GetMeshIndices()[i];
				Material* material = AssetManager::GetMaterialByIndex(m_meshMaterialIndices[i]);
				if (material) {
					renderItem.baseColorTextureIndex = material->m_baseColor;
					renderItem.normalColorTextureIndex = material->m_normal;
					renderItem.rmaColorTextureIndex = material->m_rma;
				}
				m_renderItems.push_back(renderItem);
			}
		}
	}
}


void GameObject::PrintMeshNames() {
	if (m_model) {
		std::cout << "\nModel name: " + m_model->GetName() << "\n";
		for (uint32_t meshIndex : m_model->GetMeshIndices()) {
			OpenGLDetachedMesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
			if (mesh) {
				std::cout << "Mesh index " << "- " << meshIndex << ": " << mesh->GetName() << "\n";
			}
		}
	}
}

std::vector<RenderItem>& GameObject::GetRendredItems() {
	return m_renderItems;
}
