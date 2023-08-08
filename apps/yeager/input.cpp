#include "input.h"
#include "common/common.h"
#include "engine/editor/camera.h"
#include <GLFW/glfw3.h>

float Input::lastX = WindowX / 2.0f;
float Input::lastY = WindowY / 2.0f;
bool Input::firstMouse = true;

void Input::MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (m_app->GetEditorCamera()->GetShouldMove()) {
        m_app->GetEditorCamera()->MouseCallback(firstMouse, lastX, lastY, xpos, ypos);
    }
}

void Input::ProcessInputRender(Window *window, float delta) {
    if (glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window->getWindow(), true);
    }

    if (m_current_mode == InputCurrentMode::kEditorMode) {
        if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            m_app->GetEditorCamera()->UpdatePosition(CameraPosition::kForward, delta);
        }
        if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            m_app->GetEditorCamera()->UpdatePosition(CameraPosition::kRight, delta);
        }
        if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            m_app->GetEditorCamera()->UpdatePosition(CameraPosition::kLeft, delta);
        }
        if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            m_app->GetEditorCamera()->UpdatePosition(CameraPosition::kBackward, delta);
        }
    }
}