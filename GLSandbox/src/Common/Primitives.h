#pragma once
#include "Common.h"

struct Cube {
    static std::vector<Vertex> GetVertices() {
        std::vector<Vertex> vertices;

        // (Z+)
        vertices.push_back({ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });

        // (Z-)
        vertices.push_back({ { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} });
        vertices.push_back({ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} });
        vertices.push_back({ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} });

        // (X+)
        vertices.push_back({ {0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} });
        vertices.push_back({ {0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} });
        vertices.push_back({ {0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} });
        vertices.push_back({ {0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} });

        // (X-)
        vertices.push_back({ {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
        vertices.push_back({ {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} });
        vertices.push_back({ {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });
        vertices.push_back({ {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });

        // (Y+)
        vertices.push_back({ {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });

        // (Y-)
        vertices.push_back({ {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });
        vertices.push_back({ {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });

        return vertices;
    }

    static std::vector<uint32_t> GetIndices() {
        return {
            // Front
            0, 1, 2, 2, 3, 0,

            // Back
            4, 5, 6, 6, 7, 4,

            // Right
            8, 9, 10, 10, 11, 8,

            // Left
            12, 13, 14, 14, 15, 12,

            // Top
            16, 17, 18, 18, 19, 16,

            // Bottom
            20, 21, 22, 22, 23, 20
        };
    }
};


namespace Primitives {
	inline std::vector<glm::vec3> CreateCubeVertices(float size) {
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(-size, size, size));	// Top
        vertices.push_back(glm::vec3(-size, size, -size));
        vertices.push_back(glm::vec3(size, size, -size));
        vertices.push_back(glm::vec3(size, size, size));
        vertices.push_back(glm::vec3(-size, -size, size));	// Bottom
        vertices.push_back(glm::vec3(-size, -size, -size));
        vertices.push_back(glm::vec3(size, -size, -size));
        vertices.push_back(glm::vec3(size, -size, size));
        vertices.push_back(glm::vec3(-size, size, size));	// Z front
        vertices.push_back(glm::vec3(-size, -size, size));
        vertices.push_back(glm::vec3(size, -size, size));
        vertices.push_back(glm::vec3(size, size, size));
        vertices.push_back(glm::vec3(-size, size, -size));	// Z back
        vertices.push_back(glm::vec3(-size, -size, -size));
        vertices.push_back(glm::vec3(size, -size, -size));
        vertices.push_back(glm::vec3(size, size, -size));
        vertices.push_back(glm::vec3(size, size, -size));	// X front
        vertices.push_back(glm::vec3(size, -size, -size));
        vertices.push_back(glm::vec3(size, -size, size));
        vertices.push_back(glm::vec3(size, size, size));
        vertices.push_back(glm::vec3(-size, size, -size));	// X back
        vertices.push_back(glm::vec3(-size, -size, -size));
        vertices.push_back(glm::vec3(-size, -size, size));
        vertices.push_back(glm::vec3(-size, size, size));
        return vertices;
	}

    inline std::vector<unsigned int> CreateCubeIndices() {
        return { 0, 1, 3, 1, 2, 3, 7, 5, 4, 7, 6, 5, 11, 9, 8, 11, 10, 9, 12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 23, 21, 20, 23, 22, 21 };
    }
}
