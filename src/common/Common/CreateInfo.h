#pragma once
#include <glm/glm.hpp>
#include "Camera/Camera.h"

// TODO: Create info about all models and implement it to renderer

struct VolumetricCloudsCreateInfo {
	float cloudSpeed = 450.0f;
	float coverage = 0.45f;
	float crispiness = 40.0f;
	float curliness = 0.1f;
	float density = 0.02f;
	float absorption = 0.35f;

	bool enableGodRays = false;
	bool postProcess;

	glm::vec3 lightPos = glm::vec3(0.0, 1.4, 0.0) * 1e12f + Camera::GetInstance().GetCameraPosition();
	glm::vec3 lightColor = glm::vec3(1.0f, 0.96f, 0.9f);

	glm::vec3 cloudColorTop = (glm::vec3(169.0f, 149.0f, 149.0f) * (1.5f / 255.f));
	glm::vec3 cloudColorBottom = (glm::vec3(65.0f, 70.0f, 80.0f) * (1.5f / 255.0f));

	glm::vec3 skyColorTop = glm::vec3(0.5f, 0.7f, 0.8f) * 1.05f;
	glm::vec3 skyColorBottom = glm::vec3(0.9f, 0.9f, 0.95f);

	GLuint weatherTex = 0;
	GLuint perlinTex = 0;
	GLuint worley32 = 0;
};