#pragma once

#include "Common/Common.h"
#include <random>
#include <format>
#include <regex>
#include <filesystem>

namespace Util {
	std::string Vec3ToString(glm::vec3 v);
	glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b);
	glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b);
	std::string GetFullPath(const std::filesystem::directory_entry& entry);
	const std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry);
	const std::string GetFileName(const std::string& name);
	std::string GetFileExtension(const std::filesystem::directory_entry& entry);
	std::vector<FileInfo> IterateDirectory(const std::string& dir, std::vector<std::string> extension = std::vector<std::string>());
	bool FileExist(const std::string& filePath);
}