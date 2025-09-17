#include "Camera.h"
#include "../Input/Input.h"

namespace Camera {
	Transform g_transform;
	double g_mouseX = 0;
	double g_mouseY = 0;
	double g_mouseOffsetX = 0;
	double g_mouseOffsetY = 0;
	float g_mouseSensitivity = 0.002f;
	float g_walkSpeed = 5.0f;
	GLFWwindow* g_window;

    float g_distance = 5.0f;
    float g_yaw = 0.0f;
    float g_pitch = 0.3f;

    enum class CameraMode { FIRST_PERSON, THIRD_PERSON };
    CameraMode g_cameraMode = CameraMode::FIRST_PERSON;

    glm::vec3 g_playerPos = glm::vec3(0.0f);

	void Init(GLFWwindow* window) {
		g_window = window;
		g_transform.position = glm::vec3(0.0f, 2.0f, 5.0f);
		g_transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		double x, y;
		glfwGetCursorPos(g_window, &x, &y);
		g_mouseOffsetX = x;
		g_mouseOffsetY = y;
		g_mouseX = x;
		g_mouseY = y;
	}
	void Update(float deltaTime) {
        static bool g_lastPressedFrame = false;
        bool g_pressedThisFrame = Input::KeyDown(GLFW_KEY_TAB);

        if (g_pressedThisFrame && !g_lastPressedFrame) {
            if (g_cameraMode == CameraMode::FIRST_PERSON) g_cameraMode = CameraMode::THIRD_PERSON;
            else g_cameraMode = CameraMode::FIRST_PERSON;
        }
        g_lastPressedFrame = g_pressedThisFrame;
        double x, y;
        glfwGetCursorPos(g_window, &x, &y);
        g_mouseOffsetX = x - g_mouseX;
        g_mouseOffsetY = y - g_mouseY;
        g_mouseX = x;
        g_mouseY = y;

        if (g_cameraMode == CameraMode::FIRST_PERSON) {
            g_transform.rotation.x += -g_mouseOffsetY * g_mouseSensitivity;
            g_transform.rotation.y += -g_mouseOffsetX * g_mouseSensitivity;
            g_transform.rotation.x = std::min(g_transform.rotation.x, 1.5f);
            g_transform.rotation.x = std::max(g_transform.rotation.x, -1.5f);
            glm::vec3 camRight = glm::vec3(g_transform.to_mat4()[0]);
            glm::vec3 camForward = glm::vec3(g_transform.to_mat4()[2]);
            glm::vec3 movementForwardVector = glm::normalize(glm::vec3(camForward.x, 0, camForward.z));

            // Speed factor
            float speedFactor = 1.0f;
            if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) {
                speedFactor = 0.425f;
            }
            // WSAD
            glm::vec3 displacement = glm::vec3(0);
            if (Input::KeyDown(GLFW_KEY_A)) displacement -= camRight;
            if (Input::KeyDown(GLFW_KEY_D)) displacement += camRight;
            if (Input::KeyDown(GLFW_KEY_W)) displacement -= movementForwardVector;
            if (Input::KeyDown(GLFW_KEY_S)) displacement += movementForwardVector;

            displacement *= g_walkSpeed * deltaTime * speedFactor;
            g_transform.position += displacement;

            // View height
            float heightSpeed = 3.0f;
            if (Input::KeyDown(GLFW_KEY_SPACE)) g_transform.position.y += deltaTime * heightSpeed * speedFactor;
            if (Input::KeyDown(GLFW_KEY_LEFT_CONTROL)) g_transform.position.y -= deltaTime * heightSpeed * speedFactor;

            g_playerPos = g_transform.position;
        }
        else {
            g_yaw += -g_mouseOffsetX * g_mouseSensitivity;
            g_pitch += -g_mouseOffsetY * g_mouseSensitivity;
            g_pitch = std::clamp(g_pitch, -1.5f, 1.5f);

            glm::vec3 forward = glm::normalize(glm::vec3(cos(g_yaw), 0.0f, sin(g_yaw)));
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

            glm::vec3 displacement = glm::vec3(0.0f);
            if (Input::KeyDown(GLFW_KEY_A)) displacement -= right;
            if (Input::KeyDown(GLFW_KEY_D)) displacement += right;
            if (Input::KeyDown(GLFW_KEY_W)) displacement += forward;
            if (Input::KeyDown(GLFW_KEY_S)) displacement -= forward;

            if (glm::length(displacement) > 0.0f) {
                displacement = glm::normalize(displacement);
            }

            float speedFactor = 1.0f;
            if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) {
                speedFactor = 0.425f;
            }

            g_playerPos += displacement * g_walkSpeed * deltaTime * speedFactor;

            glm::vec3 offset;
            offset.x = cos(g_pitch) * cos(g_yaw);
            offset.y = sin(g_pitch);
            offset.z = cos(g_pitch) * sin(g_yaw);
            offset = glm::normalize(offset) * g_distance;

            g_transform.position = g_playerPos - offset;
            g_transform.rotation = glm::vec3(g_pitch, g_yaw, 0.0f);
        }
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
}