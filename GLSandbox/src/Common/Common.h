#pragma once

#include <string>
#include <memory>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <numeric>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <unordered_map>

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Types.h"

#define ORANGE  glm::vec3(1, 0.647f, 0)
#define BLACK   glm::vec3(0, 0, 0)
#define WHITE   glm::vec3(1, 1, 1)
#define RED     glm::vec3(1, 0, 0)
#define GREEN   glm::vec3(0, 1, 0)
#define BLUE    glm::vec3(0, 0, 1)
#define YELLOW  glm::vec3(1, 1, 0)
#define PURPLE  glm::vec3(1, 0, 1)
#define GREY    glm::vec3(0.25f)
#define LIGHT_BLUE    glm::vec3(0, 1, 1)
#define LIGHT_GREEN   glm::vec3(0.16f, 0.78f, 0.23f)
#define LIGHT_RED     glm::vec3(0.8f, 0.05f, 0.05f)

#define PI 3.14159265359f

