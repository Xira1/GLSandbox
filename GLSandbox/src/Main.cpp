#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Input/Input.h"
#include "Camera/Camera.h"
#include "Core/Scene.hpp"
#include "Types.h"

#include "Util.hpp"

void Init(const std::string& title) {
    OpenGLBackend::Init(title);
    AssetManager::Init();
    Scene::Init();
    Input::Init(OpenGLBackend::GetWindowPtr());
    Camera::Init(OpenGLBackend::GetWindowPtr());
    Scene::CreateGameObjects();
    OpenGLRenderer::Init();
}

void Update() {
    static float deltaTime = 0;
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    OpenGLBackend::UpdateTextureBaking();
    Scene::SetMaterials();
    AssetManager::Update();
    Input::Update();
    Camera::Update(deltaTime);
    Scene::Update(deltaTime);
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

void Clear() {
    OpenGLBackend::Cleanup();
}

int main()
{
    Init("Sandbox");
    glfwSwapInterval(1);

    while (OpenGLBackend::WindowIsOpen()) {
        Update();
        Render();
    }

    Clear();
    return 0;
}