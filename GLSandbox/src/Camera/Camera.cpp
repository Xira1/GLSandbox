#include "Camera.h"
#include "../Input/Input.h"

// Orbit camera

glm::mat4 OrbitCamera::CalculateViewMatrix(const glm::vec3& target) const {
	glm::vec3 forward;
	forward.x = cos(pitch) * cos(yaw);
	forward.y = sin(pitch);
	forward.z = cos(pitch) * sin(yaw);

	glm::vec3 eye = target - glm::normalize(forward) * distance;
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	return glm::lookAt(eye, target, up);
}

void OrbitCamera::Rotate(float deltaX, float deltaY, float sensitivity, bool firstPerson) {
	yaw += deltaX * sensitivity;
	pitch += -deltaY * sensitivity;

	if (firstPerson)
		pitch = std::clamp(pitch, -1.5f, 1.5f);
	else
		pitch = std::clamp(pitch, -0.8f, 0.8f);

	yaw = NormalizeAngle(yaw);
}

void OrbitCamera::Zoom(float delta) {
	distance = std::clamp(distance + delta, 0.1f, 20.0f);
}

// Camera

void Camera::Init(GLFWwindow* window) {
	g_window = window;
	m_position = glm::vec3(0.0f, 1.0f, 5.0f);
	g_transform.position = m_position + glm::vec3(0.0f, 1.0f, 0.0f);

	double x, y;
	glfwGetCursorPos(g_window, &x, &y);
	glfwSetWindowUserPointer(window, this);
	g_mouseX = x;
	g_mouseY = y;

	glfwSetScrollCallback(window, [](GLFWwindow* win, double, double yoffset) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(win));
		if (cam->GetCameraMode() == CameraMode::THIRD_PERSON)
			cam->GetOrbitCamera().Zoom(-yoffset * 0.5f);
		});
}

void Camera::SwitchCameraMode() {
	if (g_cameraMode == CameraMode::FIRST_PERSON) {
		g_cameraMode = CameraMode::THIRD_PERSON;

		g_orbitCamera.yaw = g_transform.rotation.y;
		g_orbitCamera.pitch = g_transform.rotation.x;
		g_mouseCaptured = false;
	}
	else {
		g_cameraMode = CameraMode::FIRST_PERSON;

		g_transform.rotation.y = g_orbitCamera.yaw;
		g_transform.rotation.x = g_orbitCamera.pitch;
		g_mouseCaptured = false;
	}
}

void Camera::Update(float deltaTime) {
	static bool lastPressedTab = false;
	bool pressedTab = glfwGetKey(g_window, GLFW_KEY_TAB) == GLFW_PRESS;
	if (pressedTab && !lastPressedTab) SwitchCameraMode();
	lastPressedTab = pressedTab;

	double x, y;
	glfwGetCursorPos(g_window, &x, &y);
	g_mouseOffsetX = x - g_mouseX;
	g_mouseOffsetY = y - g_mouseY;
	g_mouseX = x;
	g_mouseY = y;

	if (!g_mouseCaptured) {
		if (std::abs(g_mouseOffsetX) > 0.0 || std::abs(g_mouseOffsetY) > 0.0)
			g_mouseCaptured = true;
		else
			g_mouseOffsetX = g_mouseOffsetY = 0.0;
	}

	g_orbitCamera.Rotate(g_mouseOffsetX, g_mouseOffsetY, g_mouseSensitivity, g_cameraMode == CameraMode::FIRST_PERSON);

	glm::mat4 camMat;
	if (g_cameraMode == CameraMode::FIRST_PERSON)
		camMat = g_transform.to_mat4();
	else {
		glm::vec3 targetCenter = m_position + glm::vec3(0.0f, 1.0f, 0.0f);
		camMat = glm::inverse(g_orbitCamera.CalculateViewMatrix(targetCenter));
	}

	// WASD
	glm::vec3 displacement(0.0f);
	if (g_cameraMode == CameraMode::FIRST_PERSON) {
		glm::vec3 camForward;
		camForward.x = cos(g_orbitCamera.pitch) * cos(g_orbitCamera.yaw);
		camForward.y = 0.0f;
		camForward.z = cos(g_orbitCamera.pitch) * sin(g_orbitCamera.yaw);
		camForward = glm::normalize(camForward);

		glm::vec3 camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));

		if (Input::KeyDown(GLFW_KEY_W)) displacement += camForward;
		if (Input::KeyDown(GLFW_KEY_S)) displacement -= camForward;
		if (Input::KeyDown(GLFW_KEY_D)) displacement += camRight;
		if (Input::KeyDown(GLFW_KEY_A)) displacement -= camRight;
	}
	else {
		glm::vec3 camForward = glm::normalize(glm::vec3(camMat[2].x, 0.0f, camMat[2].z));
		glm::vec3 camRight = glm::normalize(glm::vec3(camMat[0].x, 0.0f, camMat[0].z));

		if (Input::KeyDown(GLFW_KEY_W)) displacement -= camForward;
		if (Input::KeyDown(GLFW_KEY_S)) displacement += camForward;
		if (Input::KeyDown(GLFW_KEY_D)) displacement += camRight;
		if (Input::KeyDown(GLFW_KEY_A)) displacement -= camRight;
	}

	if (glm::length(displacement) > 0.0f) displacement = glm::normalize(displacement);

	// Speed factor
	float speedFactor = 1.0f;
	if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) speedFactor = 0.405f;
	if (Input::KeyDown(GLFW_KEY_LEFT_ALT)) speedFactor = 1.5f;

	// Height speed
	float heightSpeed = 3.0f;
	if (Input::KeyDown(GLFW_KEY_SPACE)) m_position.y += deltaTime * heightSpeed * speedFactor;
	if (Input::KeyDown(GLFW_KEY_LEFT_CONTROL)) m_position.y -= deltaTime * heightSpeed * speedFactor;

	m_position += displacement * g_walkSpeed * deltaTime * speedFactor;

	if (g_cameraMode == CameraMode::FIRST_PERSON) {
		g_transform.position = m_position + glm::vec3(0.0f, 1.0f, 0.0f);
		g_transform.rotation.x = g_orbitCamera.pitch;
		g_transform.rotation.y = g_orbitCamera.yaw;
	}
	else {
		glm::vec3 targetCenter = m_position + glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = g_orbitCamera.CalculateViewMatrix(targetCenter);
		glm::mat4 invView = glm::inverse(view);
		g_transform.position = glm::vec3(invView[3]);
		g_transform.rotation = glm::vec3(g_orbitCamera.pitch, g_orbitCamera.yaw, 0.0f);
	}
}

const glm::mat4& Camera::GetViewMatrixPlayer() const {
	if (g_cameraMode == CameraMode::FIRST_PERSON) {
		glm::vec3 dir;
		dir.x = cos(g_orbitCamera.pitch) * cos(g_orbitCamera.yaw);
		dir.y = sin(g_orbitCamera.pitch);
		dir.z = cos(g_orbitCamera.pitch) * sin(g_orbitCamera.yaw);
		dir = glm::normalize(dir);

		glm::vec3 eyePos = m_position + glm::vec3(0.0f, 1.0f, 0.0f);
		return glm::lookAt(eyePos, eyePos + dir, glm::vec3(0, 1, 0));
	}
	else {
		glm::vec3 targetCenter = m_position + glm::vec3(0.0f, 1.0f, 0.0f);
		return g_orbitCamera.CalculateViewMatrix(targetCenter);
	}
}

const glm::vec3& Camera::GetEyePosition() const {
	return m_position + glm::vec3(0.0f, 1.0f, 0.0f);
}

const glm::mat4& Camera::GetProjectionMatrix() const {
	int width, height;
	glfwGetWindowSize(g_window, &width, &height);
	return glm::perspective(1.0f, float(width) / float(height), NEAR_PLANE, FAR_PLANE);
}

const glm::mat4& Camera::GetInverseViewMatrix() const {
	return m_inverseViewMatrix;
}

const glm::mat4& Camera::GetViewMatrix() const {
	return m_viewMatrix;
}

const glm::vec3& Camera::GetPosition() const {
	return m_position;
}

const glm::vec3& Camera::GetEulerRotation() const {
	return m_rotation;
}

const glm::quat& Camera::GetQuaternionRotation() const {
	return m_rotationQ;
}

const glm::vec3& Camera::GetForward() const {
	return m_forward;
}

const glm::vec3& Camera::GetUp() const {
	return m_up;
}

const glm::vec3& Camera::GetRight() const {
	return m_right;
}

const glm::vec3 Camera::GetForwardXZ() const {
	return glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
}