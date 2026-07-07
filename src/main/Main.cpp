#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Input/Input.h"
#include "Camera/Camera.h"
#include "Core/Scene.hpp"
#include "Common/Types.h"
#include "Game/Game.h"

void Init(const std::string& title) {
    OpenGLBackend::Init(title);
    AssetManager::Init();
    Scene::Init();
    Input::Init(OpenGLBackend::GetWindowPtr());
    Camera::GetInstance().Init(OpenGLBackend::GetWindowPtr());
    Scene::CreateGameObjects();
    OpenGLRenderer::Init();
}

void Update() {
    Game::Update();
    OpenGLBackend::UpdateTextureBaking();
    Scene::SetMaterials();
    AssetManager::Update();
    Input::Update();
    Camera::GetInstance().Update(Game::GetDeltaTime());
    Scene::Update(Game::GetDeltaTime());
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

int main() {
    Init("Sandbox");
    glfwSwapInterval(1);

    while (OpenGLBackend::WindowIsOpen()) {
        Update();
        Render();
    }

    Clear();
    return 0;
}