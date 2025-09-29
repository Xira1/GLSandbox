#pragma once

#include "Common.h"
#include <random>
#include <format>
#include <regex>
#include <filesystem>


namespace Util {
	inline std::string Vec3ToString(glm::vec3 v) {
		return std::format("({}, {}, {})", v.x, v.y, v.z);
	}

	inline const std::string GetFileName(const std::string filePath) {
		std::filesystem::path p(filePath);
		return p.stem().string();
	}

	inline glm::vec3 Vec3Min(glm::vec3& a, glm::vec3& b) {
		return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

	inline glm::vec3 Vec3Max(glm::vec3& a, glm::vec3& b) {
		return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}
}