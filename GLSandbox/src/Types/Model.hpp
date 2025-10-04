#pragma once

#include "Common.h"

struct Model {
private:
	std::string m_name = "undefined";
	std::vector<uint32_t> m_meshIndices;
	bool isLoaded = false;

public:
	glm::vec3 m_aabbMin = glm::vec3(std::numeric_limits<float>::min());
	glm::vec3 m_aabbMax = glm::vec3(-std::numeric_limits<float>::max());
	std::string m_fullPath;

public:

	Model() = default;
	
	Model(std::string fullPath) {
		m_fullPath = fullPath;
		m_name = fullPath.substr(fullPath.rfind('/') + 1);
		m_name = m_name.substr(0, m_name.length() - 4);
	}

	std::vector<uint32_t>& GetMeshIndices() {
		return m_meshIndices;
	}

	size_t GetMeshCount() {
		return m_meshIndices.size();
	}

	void SetLoadStatus(bool loaded) {
		isLoaded = loaded;
	}

	bool GetLoadStatus() {
		return isLoaded;
	}

	void SetName(const std::string& name) {
		m_name = name;
	}

	void AddMeshIndex(uint32_t index) {
		m_meshIndices.push_back(index);
	}

	void SetAABB(glm::vec3 aabbMin, glm::vec3 aabbMax) {
		aabbMin = m_aabbMin;
		aabbMax = m_aabbMax;
	}

	std::string GetName() {
		return m_name;
	}
};