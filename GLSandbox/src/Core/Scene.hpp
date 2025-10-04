#pragma once

#include "../Types/GameObject.h"
#include "../Input/Input.h"

namespace Scene {
	inline std::vector<GameObject> g_gameObjects;
	inline std::vector<RenderItem> g_renderItems;

	inline void Init() {
		// TODO: ???
	}

	inline void Update(float deltaTime) {
		g_renderItems.clear();

		for (GameObject& gameObject : g_gameObjects) {
			if (!gameObject.m_model) {
				gameObject.SetModel(gameObject.m_name);
			}
			
			gameObject.UpdateRenderItems();

			if (gameObject.m_name == "Rock") {
				float amt = 0.5f;
				if (Input::KeyDown(GLFW_KEY_LEFT)) {
					gameObject.m_transform.rotation.y += deltaTime * amt;
				}
			}

			g_renderItems.insert(g_renderItems.end(), gameObject.GetRendredItems().begin(), gameObject.GetRendredItems().end());
		}
	}

	inline GameObject* GetGameObjectByName(const std::string& name) {
		for (GameObject& gameObject : g_gameObjects) {
			if (gameObject.m_name == name) {
				return &gameObject;
			}
		}

		return nullptr;
	}

	inline void AddGameObjects() {
		g_gameObjects.emplace_back();
	}

	inline void CreateGameObjects() {
		AddGameObjects();
		GameObject* rock = &g_gameObjects[g_gameObjects.size() - 1];
		rock->SetPosition(glm::vec3(0.0f));
		rock->SetModel("Rock");
		rock->SetName("Rock");

		AddGameObjects();
		GameObject* lionHead = &g_gameObjects[g_gameObjects.size() - 1];
		lionHead->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
		lionHead->SetModel("LionHead");
		lionHead->SetName("LionHead");

		AddGameObjects();
		GameObject* Tree = &g_gameObjects[g_gameObjects.size() - 1];
		Tree->SetPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
		Tree->SetModel("TreeLarge_0");
		Tree->SetName("TreeLarge_0");
	}

	inline std::vector<RenderItem> GetRenderItems() { return g_renderItems; }
}