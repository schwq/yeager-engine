#include "InputHandle.h"
#include "Application.h"
#include "Common/Common.h"
#include "Engine/Editor/Camera.h"

float Input::lastX = ygWindowWidth / 2.0f;
float Input::lastY = ygWindowHeight / 2.0f;
bool Input::firstMouse = true;
Yeager::ApplicationCore* Input::m_app = nullptr;
Input::~Input()
{
  Yeager::Log(INFO, "Destrorying InputHandle!");
}

yg_uint Input::m_framesCount = 0;
Input::Input(Yeager::ApplicationCore* app)
{
  m_app = app;
  Yeager::Log(INFO, "Input created");
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (m_app->GetCamera()->GetShouldMove()) {
    m_app->GetCamera()->MouseCallback(firstMouse, lastX, lastY, xpos, ypos);
  }
}

void Input::MakeCursorStaticAppear(bool make)
{
  if (make) {
    glfwSetInputMode(m_app->GetWindow()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    SetCursorCanDisappear(false);
  } else {
    SetCursorCanDisappear(true);
  }
}

const void Input::SetCursorCanDisappear(bool should)
{
  m_cursor_can_disappear = should;
  if (should) {
    glfwSetInputMode(m_app->GetWindow()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

bool Input::GetKeyPressed(int key)
{
  return glfwGetKey(m_app->GetWindow()->getWindow(), key) == GLFW_PRESS;
}

void Input::ProcessInputRender(Window* window, float delta)
{
  m_framesCount++;
  Interface* intr = m_app->GetInterface();
  if (glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {

    if (!intr->GetExitProgramWindowOpen()) {
      intr->SetExitProgramWindowOpen(true);
    }
  }
  if (intr->GetUserChoiceExitProgram()) {
    glfwSetWindowShouldClose(window->getWindow(), true);
  }

  if (m_app->GetMode() == Yeager::AppEditor && m_app->GetCamera()->GetShouldMove()) {
    if (GetKeyPressed(GLFW_KEY_W)) {
      m_app->GetCamera()->UpdatePosition(CameraPosition::kForward, delta);
    }
    if (GetKeyPressed(GLFW_KEY_D)) {
      m_app->GetCamera()->UpdatePosition(CameraPosition::kRight, delta);
    }
    if (GetKeyPressed(GLFW_KEY_A)) {
      m_app->GetCamera()->UpdatePosition(CameraPosition::kLeft, delta);
    }
    if (GetKeyPressed(GLFW_KEY_S)) {
      m_app->GetCamera()->UpdatePosition(CameraPosition::kBackward, delta);
    }
  }
  if (m_app->GetMode() == Yeager::AppEditor) {
    if (((glfwGetKey(window->getWindow(), GLFW_KEY_E) == GLFW_PRESS) && m_framesCount % 5 == 0) &&
        m_cursor_can_disappear) {
      if (m_app->GetCamera()->GetShouldMove()) {
        m_app->GetCamera()->SetShouldMove(false);
        firstMouse = true;
        glfwSetInputMode(m_app->GetWindow()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      } else {

        m_app->GetCamera()->SetShouldMove(true);

        glfwSetInputMode(m_app->GetWindow()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
    }
  }
}
