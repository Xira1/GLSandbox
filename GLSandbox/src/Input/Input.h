#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Input {
	void Init(GLFWwindow* window);
	void Update();
	bool KeyPressed(unsigned int keycode);
	bool KeyDown(unsigned int keycode);
}

