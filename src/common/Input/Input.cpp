#include "Input.h"

namespace Input {
    bool _keyPressed[372];
    bool _keyDown[372];
    bool _keyDownLastFrame[372];
    double _mouseX = 0;
    double _mouseY = 0;
    double _mouseOffsetX = 0;
    double _mouseOffsetY = 0;
    int _mouseWheelValue = 0;
    int _sensitivity = 100;
    int _scrollWheelYOffset = 0;
    GLFWwindow* _window;

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void Init(GLFWwindow* window) {
        double x, y;
        _window = window;
        glfwSetScrollCallback(_window, scroll_callback);
        glfwGetCursorPos(_window, &x, &y);
        _mouseOffsetX = x;
        _mouseOffsetY = y;
        _mouseX = x;
        _mouseY = y;
    }

    void Update() {
        // Keyboard
        for (int i = 32; i < 349; i++) {
            // down
            if (glfwGetKey(_window, i) == GLFW_PRESS)
                _keyDown[i] = true;
            else
                _keyDown[i] = false;

            // press
            if (_keyDown[i] && !_keyDownLastFrame[i])
                _keyPressed[i] = true;
            else
                _keyPressed[i] = false;
            _keyDownLastFrame[i] = _keyDown[i];
        }
    }

    bool KeyPressed(unsigned int keycode) {
        return _keyPressed[keycode];
    }
    bool KeyDown(unsigned int keycode) {
        return _keyDown[keycode];
    }

    void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
        _scrollWheelYOffset = (int)yoffset;
    }
}