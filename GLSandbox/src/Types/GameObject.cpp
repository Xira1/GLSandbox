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
	Model* model = AssetManager::GetModelByName(name);

	if (model) {
		m_model = model;
		m_meshMaterialIndices.resize(m_model->GetMeshCount());
		std::fill(m_meshMaterialIndices.begin(), m_meshMaterialIndices.end(), -1);
	}
	else {
		PendingModel pending;
		pending.onLoaded = [this](Model* loadedModel) {
			if (loadedModel->GetName() == m_name) {
				this->m_model = loadedModel;
				this->m_meshMaterialIndices.resize(m_model->GetMeshCount(), -1);
			}
		};
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
				m_renderItems.push_back(renderItem);
			}
		}
	}
}

void GameObject::PrintMeshNames() {
	if (m_model) {
		std::cout << m_model->GetName() << "\n";
		for (uint32_t meshIndex : m_model->GetMeshIndices()) {
			OpenGLDetachedMesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
			if (mesh) {
				std::cout << "-" << meshIndex << ": " << mesh->GetName() << "\n";
			}
		}
	}
}

std::vector<RenderItem>& GameObject::GetRendredItems() {
	return m_renderItems;
}
