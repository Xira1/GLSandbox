#pragma once

#include <string>
#include <memory>
#include <vector>
#include <future>
#include <thread>
#include <numeric>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <fstream>

#include <algorithm>
#include <numeric>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Types.h"

#define PINK                glm::vec4(0.98f, 0.06f, 0.75f, 1.0f)
#define ORANGE              glm::vec4(1.00f, 0.65f, 0.00f, 1.0f)
#define BLACK               glm::vec4(0.00f, 0.00f, 0.00f, 1.0f)
#define WHITE               glm::vec4(1.00f, 1.00f, 1.00f, 1.0f)
#define RED                 glm::vec4(1.00f, 0.00f, 0.00f, 1.0f)
#define GREEN               glm::vec4(0.00f, 1.00f, 0.00f, 1.0f)
#define BLUE                glm::vec4(0.00f, 0.00f, 1.00f, 1.0f)
#define YELLOW              glm::vec4(1.00f, 1.00f, 0.00f, 1.0f)
#define PURPLE              glm::vec4(1.00f, 0.00f, 1.00f, 1.0f)
#define GREY                glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)
#define LIGHT_BLUE          glm::vec4(0.00f, 1.00f, 1.00f, 1.0f)
#define LIGHT_GREEN         glm::vec4(0.16f, 0.78f, 0.23f, 1.0f)
#define LIGHT_RED           glm::vec4(0.80f, 0.05f, 0.05f, 1.0f)
#define TRANSPARENT         glm::vec4(0.00f, 0.00f, 0.00f, 0.0f)
#define GRID_COLOR          glm::vec4(0.50f, 0.50f, 0.60f, 1.0f)
#define OUTLINE_COLOR       glm::vec4(1.00f, 0.50f, 0.00f, 0.0f)
#define DEFAULT_LIGHT_COLOR glm::vec4(1.00f, 0.7799999713897705f, 0.5289999842643738f, 1.0f)

#define PI 3.14159265359f

