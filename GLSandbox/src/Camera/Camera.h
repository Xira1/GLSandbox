#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Types.h"

#define NEAR_PLANE 0.01f
#define FAR_PLANE 1000.0f

namespace Camera {
	extern enum class CameraMode { FIRST_PERSON, THIRD_PERSON };
	extern CameraMode g_cameraMode;
	extern glm::vec3 g_playerPos;

	void Init(GLFWwindow* window);
	void Update(float deltaTime);
	void SwitchCameraMode();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetViewMatrixPlayer();
	glm::vec3 GetLookDirection();
	glm::vec3 GetEyePosition();
	glm::mat4 GetInverseViewMatrix();
	glm::vec3 GetViewPos();
	Transform GetTransform();
	glm::vec3 GetViewRotation();
	glm::vec3 GetForward();
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::vec3 GetPlayerPos();
	CameraMode GetCameraMode();
}


// TODO: rewrite Camera struct

//struct CameraStruct {
//public:
//	void Update();
//	void SetPosition(glm::vec3 position);
//	void SetRotation(glm::vec3 rotation);
//	const float GetPitch() const { return m_rotation.x; }
//	const float GetYaw() const { return m_rotation.y; }
//
//private:
//	glm::vec3 m_position = glm::vec3(0.0f);
//	glm::vec3 m_rotation = glm::vec3(0.0f);
//	glm::quat m_rotationQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
//	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
//	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
//	glm::vec3 m_forwardXZ = glm::vec3(0.0f, 0.0f, 1.0f);
//	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
//	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
//	glm::mat4 m_inverseViewMatrix = glm::mat4(1.0f);
//	float minPitch = -1.5;
//	float maxPitch = 1.5;
//};