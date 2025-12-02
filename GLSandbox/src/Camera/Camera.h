#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Types.h"
#include "Enums.h"
#include "Singleton.h"

#define NEAR_PLANE 0.01f
#define FAR_PLANE 1000.0f

struct OrbitCamera {
public:
    float yaw, pitch, distance;

	OrbitCamera() : distance(5.0f), yaw(-glm::half_pi<float>()), pitch(0.0f) {}

	glm::mat4 CalculateViewMatrix(const glm::vec3& target) const;
	void Rotate(float deltaX, float deltaY, float sensitivity, bool firstPerson = false);
	void Zoom(float delta);

private:
	float NormalizeAngle(float angle) const {
		while (angle > glm::pi<float>()) angle -= glm::two_pi<float>();
		while (angle < -glm::pi<float>()) angle += glm::two_pi<float>();
		return angle;
	}
};

struct Camera : public CoreUtils::Singleton<Camera> {
public:
	void Init(GLFWwindow* window);
    void Update(float deltaTime);
	void SwitchCameraMode();
    glm::mat4 GetViewMatrixPlayer();
    glm::vec3 GetCameraPosition();
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix();
    const glm::mat4& GetInverseViewMatrix() const;
    const glm::vec3& GetEyePosition() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetEulerRotation() const;
    const glm::quat& GetQuaternionRotation() const;
    const glm::vec3& GetForward() const;
    const glm::vec3& GetUp() const;
    const glm::vec3& GetRight() const;
    const glm::vec3 GetForwardXZ() const;
    const float GetOrbitCameraDistance() const { return g_orbitCamera.distance; }
    const float GetPitch() const { return m_rotation.x; }
    const float GetYaw() const { return m_rotation.y; }
    Transform& GetTransform() { return g_transform; }
    CameraMode& GetCameraMode() { return g_cameraMode; }
    OrbitCamera& GetOrbitCamera() { return g_orbitCamera; }

private:
    CameraMode g_cameraMode = CameraMode::FIRST_PERSON;
    OrbitCamera g_orbitCamera;
    Transform g_transform;

    double g_mouseX = 0, g_mouseY = 0, g_mouseOffsetX = 0, g_mouseOffsetY = 0;
    float g_mouseSensitivity = 0.002f;
    float g_walkSpeed = 5.0f;
    GLFWwindow* g_window;
    bool g_mouseCaptured = false;

    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::quat m_rotationQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projMatrix = glm::mat4(1.0f);
    glm::mat4 m_inverseViewMatrix = glm::mat4(1.0f);
    glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
};