#include "Room.h"

#include "Common.h"
#include "../../AssetManagement/AssetManager.h"

namespace HardcodedRoom {
	inline float roomWidth = 12.0f;
	inline float roomDepth = 12.0f;
	inline float roomHeight = 6.0f;
	inline float roomY = 0;
	inline glm::vec3 bottomFrontLeft = glm::vec3(-roomWidth / 2, roomY, -roomDepth / 2);
	inline glm::vec3 bottomFrontRight = glm::vec3(roomWidth / 2, roomY, -roomDepth / 2);
	inline glm::vec3 bottomBackLeft = glm::vec3(-roomWidth / 2, roomY, roomDepth / 2);
	inline glm::vec3 bottomBackRight = glm::vec3(roomWidth / 2, roomY, roomDepth / 2);
	inline glm::vec3 topFrontLeft = bottomFrontLeft + glm::vec3(0, roomHeight, 0);
	inline glm::vec3 topFrontRight = bottomFrontRight + glm::vec3(0, roomHeight, 0);
	inline glm::vec3 topBackLeft = bottomBackLeft + glm::vec3(0, roomHeight, 0);
	inline glm::vec3 topBackRight = bottomBackRight + glm::vec3(0, roomHeight, 0);
	inline glm::vec3 zPos = glm::vec3(0, 0, 1);
	inline glm::vec3 zNeg = glm::vec3(0, 0, -1);
	inline glm::vec3 xPos = glm::vec3(1, 0, 0);
	inline glm::vec3 xNeg = glm::vec3(-1, 0, 0);
	inline glm::vec3 up = glm::vec3(0, 1, 0);
	inline glm::vec3 down = glm::vec3(0, -1, 0);

	void LoadHardcodedModelRoom() {
		Model* model = AssetManager::CreateModel("Room");
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

        // Floor
        vertices = {};
        vertices.push_back(Vertex(bottomFrontLeft, up, glm::vec2(0, 0)));
        vertices.push_back(Vertex(bottomFrontRight, up, glm::vec2(roomWidth, 0)));
        vertices.push_back(Vertex(bottomBackLeft, up, glm::vec2(0, roomDepth)));
        vertices.push_back(Vertex(bottomBackRight, up, glm::vec2(roomWidth, roomDepth)));
        indices = { 0, 2, 1, 2, 3, 1 };
        model->AddMeshIndex(AssetManager::CreateMesh("Floor", vertices, indices));
        // Ceiling
        vertices = {};
        vertices.push_back(Vertex(topFrontLeft, down, glm::vec2(0, 0)));
        vertices.push_back(Vertex(topFrontRight, down, glm::vec2(roomWidth, 0)));
        vertices.push_back(Vertex(topBackLeft, down, glm::vec2(0, roomDepth)));
        vertices.push_back(Vertex(topBackRight, down, glm::vec2(roomWidth, roomDepth)));
        indices = { 0, 1, 2, 2, 1, 3 };
        model->AddMeshIndex(AssetManager::CreateMesh("Ceiling", vertices, indices));
        // Z Pos
        vertices = {};
        vertices.push_back(Vertex(bottomFrontLeft, zPos, glm::vec2(0, 0)));
        vertices.push_back(Vertex(bottomFrontRight, zPos, glm::vec2(roomWidth / roomHeight, 0)));
        vertices.push_back(Vertex(topFrontLeft, zPos, glm::vec2(0, -1)));
        vertices.push_back(Vertex(topFrontRight, zPos, glm::vec2(roomWidth / roomHeight, -1)));
        indices = { 0, 1, 2, 2, 1, 3 };
        model->AddMeshIndex(AssetManager::CreateMesh("WallZPos", vertices, indices));
        // Z Neg
        vertices = {};
        vertices.push_back(Vertex(bottomBackLeft, zNeg, glm::vec2(0, 0)));
        vertices.push_back(Vertex(bottomBackRight, zNeg, glm::vec2(roomWidth / roomHeight, 0)));
        vertices.push_back(Vertex(topBackLeft, zNeg, glm::vec2(0, -1)));
        vertices.push_back(Vertex(topBackRight, zNeg, glm::vec2(roomWidth / roomHeight, -1)));
        indices = { 0, 2, 1, 2, 3, 1 };
        model->AddMeshIndex(AssetManager::CreateMesh("WallZNeg", vertices, indices));
        // X Pos
        vertices = {};
        vertices.push_back(Vertex(bottomFrontRight, xPos, glm::vec2(0, 0)));
        vertices.push_back(Vertex(bottomBackRight, xPos, glm::vec2(roomDepth / roomHeight, 0)));
        vertices.push_back(Vertex(topFrontRight, xPos, glm::vec2(0, -1)));
        vertices.push_back(Vertex(topBackRight, xPos, glm::vec2(roomDepth / roomHeight, -1)));
        indices = { 0, 1, 2, 2, 1, 3 };
        model->AddMeshIndex(AssetManager::CreateMesh("WallXPos", vertices, indices));
        // X Neg
        vertices = {};
        vertices.push_back(Vertex(bottomFrontLeft, xNeg, glm::vec2(0, 0)));
        vertices.push_back(Vertex(bottomBackLeft, xNeg, glm::vec2(roomDepth / roomHeight, 0)));
        vertices.push_back(Vertex(topFrontLeft, xNeg, glm::vec2(0, -1)));
        vertices.push_back(Vertex(topBackLeft, xNeg, glm::vec2(roomDepth / roomHeight, -1)));
        indices = { 0, 2, 1, 2, 3, 1 };
        model->AddMeshIndex(AssetManager::CreateMesh("WallXNeg", vertices, indices));
	}
}