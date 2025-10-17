#pragma once

#include "../Types/GameObject.h"
#include "../World/Room/Room.h"
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
			gameObject.UpdateRenderItems();

			/*if (gameObject.m_name == "Sofa_1") {
				float amt = 1.5f;
				if (Input::KeyDown(GLFW_KEY_LEFT)) {
					gameObject.m_transform.rotation.y += deltaTime * amt;
					std::cout << gameObject.m_transform.rotation.y << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_RIGHT)) {
					gameObject.m_transform.rotation.y -= deltaTime * amt;
					std::cout << gameObject.m_transform.rotation.y << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_UP)) {
					gameObject.m_transform.position.y += deltaTime * amt;
					std::cout << gameObject.m_transform.position.y << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_DOWN)) {
					gameObject.m_transform.position.y -= deltaTime * amt;
					std::cout << gameObject.m_transform.position.y << "\n";
				}
			}*/

			if (gameObject.m_name == "Lattern") {
				float amt = 1.5f;
				if (Input::KeyDown(GLFW_KEY_LEFT)) {
					gameObject.m_transform.position.x -= deltaTime * amt;
					std::cout << gameObject.m_transform.position.x << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_RIGHT)) {
					gameObject.m_transform.position.x += deltaTime * amt;
					std::cout << gameObject.m_transform.position.x << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_UP)) {
					gameObject.m_transform.position.y += deltaTime * amt;
					std::cout << gameObject.m_transform.position.y << "\n";
				}
				if (Input::KeyDown(GLFW_KEY_DOWN)) {
					gameObject.m_transform.position.y -= deltaTime * amt;
					std::cout << gameObject.m_transform.position.y << "\n";
				}
			}

			// Debug
			if (Input::KeyPressed(GLFW_KEY_P)) {
				gameObject.PrintMeshNames();
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

	inline void SetMaterials() {
		GameObject* Sofa = GetGameObjectByName("Sofa_1");
		if (Sofa) {
			Sofa->SetMeshMaterialByMeshName("set3_6", "Sofa_1");
		}

		GameObject* ChristmasTree = GetGameObjectByName("ChristmasTree");
		if (ChristmasTree) {

		}

		GameObject* CatStatue = GetGameObjectByName("CatStatue");
		if (CatStatue) {
			CatStatue->SetMeshMaterialByMeshName("CatMesh", "CatStatue");
		}
	}

	inline void CreateGameObjects() {
		AddGameObjects();
		GameObject* Room = &g_gameObjects[g_gameObjects.size() - 1];
		Room->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		Room->SetModel("Room");
		Room->SetMeshMaterialByMeshName("Floor", "Floor");
		Room->SetMeshMaterialByMeshName("Ceiling", "Ceiling");
		Room->SetMeshMaterialByMeshName("WallZPos", "Wall");
		Room->SetMeshMaterialByMeshName("WallZNeg", "Wall");
		Room->SetMeshMaterialByMeshName("WallXPos", "Wall");
		Room->SetMeshMaterialByMeshName("WallXNeg", "Wall");
		Room->SetName("Room");

		AddGameObjects();
		GameObject* Sofa = &g_gameObjects[g_gameObjects.size() - 1];
		Sofa->SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));
		Sofa->SetRotation(glm::vec3(0.0f, -0.88f, 0.0f));
		Sofa->SetModel("Sofa_1");
		Sofa->SetMeshMaterialByMeshName("set3_6", "Sofa_1");
		Sofa->SetName("Sofa_1");

		AddGameObjects();
		GameObject* Lattern = &g_gameObjects[g_gameObjects.size() - 1];
		Lattern->SetPosition(glm::vec3(2.2f, 0.66f, -0.88f));
		//Lattern->SetPosition(glm::vec3(0.0f, 2.0f, 3.0f));
		Lattern->SetRotation(glm::vec3(0.0f, -0.33f, 0.0f));
		Lattern->SetModel("Lattern");
		Lattern->SetMeshMaterialByMeshName("Mesh", "Lattern_01");
		Lattern->SetMeshMaterialByMeshName("Glass", "LatternOpacity");
		Lattern->SetName("Lattern");

		AddGameObjects();
		GameObject* CatStatue = &g_gameObjects[g_gameObjects.size() - 1];
		CatStatue->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		CatStatue->SetRotation(glm::vec3(0.0f, 0.44f, 0.0f));
		CatStatue->SetModel("CatStatue");
		CatStatue->SetMeshMaterialByMeshName("CatMesh", "CatStatue");
		CatStatue->SetName("CatStatue");

		AddGameObjects();
		GameObject* ChristmasTree = &g_gameObjects[g_gameObjects.size() - 1];
		ChristmasTree->SetPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
		ChristmasTree->SetRotation(glm::vec3(0.0f, -0.88f, 0.0f));
		ChristmasTree->SetSize(glm::vec3(0.67f));
		ChristmasTree->SetModel("ChristmasTree");
		ChristmasTree->SetName("ChristmasTree");
	}

	inline std::vector<RenderItem> GetRenderItems() { return g_renderItems; }
}