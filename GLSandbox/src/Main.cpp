#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/GL_renderer.h"
#include "Input/Input.h"
#include "Core/Camera.h"
#include "Types.h"

void Init(const std::string& title) {
    OpenGLBackend::Init(title);
    Input::Init(OpenGLBackend::GetWindowPtr());
    Camera::Init(OpenGLBackend::GetWindowPtr());
    OpenGLRenderer::Init();
}

void Update() {
    static float deltaTime = 0;
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    Input::Update();
    Camera::Update(deltaTime);
    if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(OpenGLBackend::GetWindowPtr(), true);
    }
    if (Input::KeyPressed(GLFW_KEY_F)) {
        OpenGLBackend::ToggleFullscreen();
    }
    if (Input::KeyPressed(GLFW_KEY_H)) {
        OpenGLRenderer::LoadShaders();
    }
}

void Render() {
    OpenGLRenderer::RenderFrame();
}

int main()
{
    Init("Sandbox");
    glfwSwapInterval(1);

    while (OpenGLBackend::WindowIsOpen()) {
        Update();
        Render();
    }

    OpenGLBackend::Cleanup();
    return 0;
}