#include "Camera.h"
#include "../Input/Input.h"


struct OrbitCamera {
public:
	float yaw, pitch, distance;

	OrbitCamera() : distance(5.0f), yaw(-glm::half_pi<float>()), pitch(0.0f) {}

	glm::mat4 CalculateViewMatrix(const glm::vec3& target) const {
		glm::vec3 forward;
		forward.x = cos(pitch) * cos(yaw);
		forward.y = sin(pitch);
		forward.z = cos(pitch) * sin(yaw);

		glm::vec3 eye = target - glm::normalize(forward) * distance;
		glm::vec3 up(0.0f, 1.0f, 0.0f);

		return glm::lookAt(eye, target, up);
	}

	void Rotate(float deltaX, float deltaY, float sensitivity, bool firstPerson = false) {
		yaw += deltaX * sensitivity;
		pitch += -deltaY * sensitivity;

		if (firstPerson) 
			pitch = std::clamp(pitch, -1.5f, 1.5f);
		else            
			pitch = std::clamp(pitch, -0.8f, 0.8f);

		yaw = NormalizeAngle(yaw);
	}

	void Zoom(float delta) { 
		distance = std::clamp(distance + delta, 0.1f, 20.0f); 
	}

private:
	float NormalizeAngle(float angle) const {
		while (angle > glm::pi<float>()) angle -= glm::two_pi<float>();
		while (angle < -glm::pi<float>()) angle += glm::two_pi<float>();
		return angle;
	}
};

namespace Camera {
	OrbitCamera g_orbitCamera;
	Transform g_transform;
	double g_mouseX = 0, g_mouseY = 0, g_mouseOffsetX = 0, g_mouseOffsetY = 0;
	float g_mouseSensitivity = 0.002f;
	float g_walkSpeed = 5.0f;
	GLFWwindow* g_window;
	bool g_mouseCaptured = false;

	glm::vec3 g_playerPos;
	CameraMode g_cameraMode = CameraMode::FIRST_PERSON;

	void Init(GLFWwindow* window) {
		g_window = window;
		g_playerPos = glm::vec3(0.0f, 1.0f, 5.0f);
		g_transform.position = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);

		double x, y;
		glfwGetCursorPos(g_window, &x, &y);
		g_mouseX = x;
		g_mouseY = y;

		glfwSetScrollCallback(window, [](GLFWwindow*, double, double yoffset) {
			if (g_cameraMode == CameraMode::THIRD_PERSON) g_orbitCamera.Zoom(-yoffset * 0.5f);
			});
	}

	void SwitchCameraMode() {
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

	void Update(float deltaTime) {
		static bool lastPressedTab = false;
		bool pressedTab = Input::KeyPressed(GLFW_KEY_TAB);
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
				g_mouseOffsetX = 0.0;
		}

		g_orbitCamera.Rotate(g_mouseOffsetX, g_mouseOffsetY, g_mouseSensitivity, g_cameraMode == CameraMode::FIRST_PERSON);

		glm::mat4 camMat;
		if (g_cameraMode == CameraMode::FIRST_PERSON)
			camMat = g_transform.to_mat4();
		else {
			glm::vec3 targetCenter = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
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
		if (Input::KeyDown(GLFW_KEY_SPACE)) g_playerPos.y += deltaTime * heightSpeed * speedFactor;
		if (Input::KeyDown(GLFW_KEY_LEFT_CONTROL)) g_playerPos.y -= deltaTime * heightSpeed * speedFactor;

		g_playerPos += displacement * g_walkSpeed * deltaTime * speedFactor;

		if (g_cameraMode == CameraMode::FIRST_PERSON) {
			g_transform.position = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
			g_transform.rotation.x = g_orbitCamera.pitch;
			g_transform.rotation.y = g_orbitCamera.yaw;
		}
		else {
			glm::vec3 targetCenter = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
			glm::mat4 view = g_orbitCamera.CalculateViewMatrix(targetCenter);
			glm::mat4 invView = glm::inverse(view);
			g_transform.position = glm::vec3(invView[3]);
			g_transform.rotation = glm::vec3(g_orbitCamera.pitch, g_orbitCamera.yaw, 0.0f);
		}
	}

	glm::mat4 GetViewMatrixPlayer() {
		if (g_cameraMode == CameraMode::FIRST_PERSON) {
			glm::vec3 dir;
			dir.x = cos(g_orbitCamera.pitch) * cos(g_orbitCamera.yaw);
			dir.y = sin(g_orbitCamera.pitch);
			dir.z = cos(g_orbitCamera.pitch) * sin(g_orbitCamera.yaw);
			dir = glm::normalize(dir);

			glm::vec3 eyePos = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
			return glm::lookAt(eyePos, eyePos + dir, glm::vec3(0, 1, 0));
		}
		else {
			glm::vec3 targetCenter = g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
			return g_orbitCamera.CalculateViewMatrix(targetCenter);
		}
	}

	glm::vec3 GetLookDirection() {
		glm::vec3 direction;
		direction.x = cos(g_orbitCamera.pitch) * cos(g_orbitCamera.yaw);
		direction.y = sin(g_orbitCamera.pitch);
		direction.z = cos(g_orbitCamera.pitch) * sin(g_orbitCamera.yaw);
		return glm::normalize(direction);
	}

	glm::vec3 GetEyePosition() {
		return g_playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
	}

	glm::mat4 GetProjectionMatrix() {
		int width, height;
		glfwGetWindowSize(g_window, &width, &height);
		return glm::perspective(1.0f, float(width) / float(height), NEAR_PLANE, FAR_PLANE);
	}

	glm::mat4 GetViewMatrix() {
		return glm::inverse(g_transform.to_mat4());
	}

	glm::mat4 GetInverseViewMatrix() {
		return g_transform.to_mat4();
	}

	glm::vec3 GetViewPos() {
		return g_transform.position;
	}

	Transform GetTransform() {
		return g_transform;
	}

	glm::vec3 GetViewRotation() {
		return g_transform.rotation;
	}

	glm::vec3 GetForward() {
		return glm::normalize(glm::vec3(GetViewMatrix()[2]));
	}

	glm::vec3 GetRight() {
		return glm::normalize(glm::vec3(GetViewMatrix()[0]));
	}

	glm::vec3 GetUp() {
		return glm::normalize(glm::vec3(GetViewMatrix()[1]));
	}

	glm::vec3 GetPlayerPos() {
		return g_playerPos;
	}

	CameraMode GetCameraMode() {
		return g_cameraMode;
	}
}