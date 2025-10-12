#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Common.h"

#include "Types/GL_texture.h"
#include "../Types/Texture.h"

enum class WindowedMode { WINDOWED, FULLSCREEN };

namespace OpenGLBackend {
	void Init(std::string title);
	void ToggleFullscreen();
	void ForceCloseWindow();
	void SetWindowedMode(const WindowedMode& windowedMode);
	bool WindowIsOpen();
	double GetCursorX();
	double GetCursorY();
	int GetWindowWidth();
	int GetWindowHeight();
	void SwapBuffersPollEvents();
	GLFWwindow* GetWindowPtr();

	// Textures
	void AllocateTextureMemory(Texture& texture);
	void ImmediateBake(QueuedTextureBake& queuedTextureBake);
	void UpdateTextureBaking();
	void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake);

	void Cleanup();
}