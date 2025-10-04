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

	inline glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b) {
		return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

	inline glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b) {
		return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	inline std::string GetFullPath(const std::filesystem::directory_entry& entry) {
		return entry.path().string();
	}

	inline const std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry) {
		return entry.path().stem().string();
	}

	inline const std::string GetFileName(const std::string& name) {
		return std::filesystem::path(name).stem().string();
	}

	inline std::string GetFileExtension(const std::filesystem::directory_entry& entry) {
		return entry.path().extension().string().substr(1);
	}

	inline std::vector<FileInfo> IterateDirectory(const std::string& dir, std::vector<std::string> extension = std::vector<std::string>()) {
		std::vector<FileInfo> fileInfoList;

		if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
			std::cout << "Util::IterateDirectory() failed " << dir << " does not exist\n";
			return fileInfoList;
		}

		auto entries = std::filesystem::directory_iterator(dir);
		for (const auto& entry : entries) {
			if (!std::filesystem::is_regular_file(entry)) {
				continue;
			}

			FileInfo fileInfo;
			fileInfo.path = Util::GetFullPath(entry);
			fileInfo.name = Util::GetFileNameWithoutExtension(entry);
			fileInfo.ext = Util::GetFileExtension(entry);
			fileInfo.dir = dir;
			if (extension.empty()) {
				fileInfoList.push_back(fileInfo);
			}
			else {
				for (std::string& ext : extension) {
					if (ext == fileInfo.ext) {
						fileInfoList.push_back(fileInfo);
						break;
					}
				}
			}
		}

		return fileInfoList;
	}
}