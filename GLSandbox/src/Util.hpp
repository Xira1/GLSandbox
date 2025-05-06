#pragma once

#include "Common.h"
#include <random>
#include <format>
#include <regex>

namespace Util {
	inline std::string Vec3ToString(glm::vec3 v) {
		return std::format("({}, {}, {})", v.x, v.y, v.z);
	}
}