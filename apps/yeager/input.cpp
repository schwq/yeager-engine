#include "input.h"
#include "common/common.h"
#include "engine/editor/camera.h"
#include <GLFW/glfw3.h>

float Input::lastX = kWindowX / 2.0f;
float Input::lastY = kWindowY / 2.0f;
bool Input::firstMouse = true;

Application *Input::m_app = nullptr;
uint Input::m_framesCount = 0;
Input::Input(Application *app) { m_app = app; }

void Input::MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
  if (m_app->GetEditorCamera()->GetShouldMove()) {
    m_app->GetEditorCamera()->MouseCallback(firstMouse, lastX, lastY, xpos,
                                            ypos);
  }
}

void Input::ProcessInputRender(Window *window, float delta)
{
  m_framesCount++;

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
      m_app->GetEditorCamera()->UpdatePosition(CameraPosition::kBackward,
                                               delta);
    }
    if ((glfwGetKey(window->getWindow(), GLFW_KEY_E) == GLFW_PRESS) &&
        m_framesCount % 5 == 0) {
      VLOG_F(INFO, "Key E pressed");
      if (m_app->GetEditorCamera()->GetShouldMove()) {
        m_app->GetEditorCamera()->SetShouldMove(false);
        firstMouse = true;
        glfwSetInputMode(m_app->GetWindowManager()->getWindow(), GLFW_CURSOR,
                         GLFW_CURSOR_NORMAL);
      }
      else {
        m_app->GetEditorCamera()->SetShouldMove(true);
        glfwSetInputMode(m_app->GetWindowManager()->getWindow(), GLFW_CURSOR,
                         GLFW_CURSOR_DISABLED);
      }
    }
  }
}
