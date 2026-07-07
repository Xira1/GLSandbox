#include "Game.h"

#include <GLFW/glfw3.h>

namespace Game {
	float g_deltaTime = 0;

	void Create() {

	}

	void Update() {
		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		g_deltaTime = static_cast<float>(currentTime - lastTime);
		lastTime = currentTime;
	}

	float GetDeltaTime() {
		return g_deltaTime;
	}
}