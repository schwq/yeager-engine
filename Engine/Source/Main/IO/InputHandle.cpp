#include "InputHandle.h"
#include "Common/Utils/Common.h"
#include "Editor/Camera/Camera.h"
#include "Main/Core/Application.h"
using namespace Yeager;

std::vector<KeyMap> Yeager::KeyMapping = {KeyMap(GLFW_MOUSE_BUTTON_1), KeyMap(GLFW_MOUSE_BUTTON_2),
                                          KeyMap(GLFW_KEY_E),          KeyMap(GLFW_KEY_LEFT_CONTROL),
                                          KeyMap(GLFW_KEY_P),          KeyMap(GLFW_KEY_R)};
KeyMap* Yeager::FindKeyMap(uint8_t key)
{
  for (auto& keymap : KeyMapping) {
    if (key == keymap.GlfwMacro) {
      return &keymap;
    }
  }
  static_assert("Cannot find key associated to keymapping!");
  return &KeyMapping[0];  // MSVC dont throw warning!
}

float Input::m_LastMouseWidth = 0;
float Input::m_LastMouseHeight = 0;
bool Input::m_FirstMouse = true;
CameraCursorLastState Input::m_LastState;
bool Input::m_CursorShouldAppear = true;
bool Input::m_SilentInputHandling = false;

void Input::KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (m_SilentInputHandling)
    return;

  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) {
    switch (key) {
      case GLFW_KEY_E:
        if (FindKeyMap(GLFW_KEY_E)->AddStateAwaitAction(action)) {
          if (m_Application->GetCamera()->GetShouldMove()) {
            m_Application->GetCamera()->SetShouldMove(false);
            m_FirstMouse = true;
            SetCursorAppear(true);
          } else {
            m_Application->GetCamera()->SetShouldMove(true);
            SetCursorAppear(false);
          }
        }
        break;
      case GLFW_KEY_P:
        if (FindKeyMap(GLFW_KEY_P)->AddStateAwaitAction(action) && YEAGER_ALT_SHIFT(mods)) {
          Yeager::Interface* inter = m_Application->GetInterface();
          if (!inter->GetDebugControlWindowOpen()) {  // If already open, close it, if its closed, it open it
            inter->SetDebugControlWindowOpen(true);   // Debug control window open
          } else {
            inter->SetDebugControlWindowOpen(false);
          }
        }
        break;
      case GLFW_KEY_R:
        if (FindKeyMap(GLFW_KEY_R)->AddStateAwaitAction(action) &&
            YEAGER_ALT_SHIFT(mods)) {  // Quick Troubleshoot the engine
          Yeager::BaseCamera* camera = m_Application->GetCamera();
          camera->SetShouldMove(false);  // release the camera
          SetCursorAppear(true);         // release the cursor
        }
        break;
      case GLFW_KEY_S:
        if (FindKeyMap(GLFW_KEY_S)->AddStateAwaitAction(action) && mods & GLFW_MOD_CONTROL) {
          m_Application->GetScene()->Save();  // Ctrl + S saves the scene
          Yeager::LogDebug(INFO, "Scene saved!");
        }
    }
  }
}

void Input::MouseKeyCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) {
    switch (button) {
      case GLFW_MOUSE_BUTTON_2:
        if (FindKeyMap(GLFW_MOUSE_BUTTON_2)->AddStateAwaitAction(action)) {}
        break;
    }
  }
}

Yeager::ApplicationCore* Input::m_Application = YEAGER_NULLPTR;
Input::~Input()
{
  Yeager::Log(INFO, "Destrorying InputHandle!");
}

Uint Input::m_FramesCount = 0;
Input::Input(Yeager::ApplicationCore* app)
{
  m_Application = app;
  Yeager::Log(INFO, "Input created");
}

void Input::InitializeCallbacks()
{
  glfwSetMouseButtonCallback(m_Application->GetWindow()->GetGLFWwindow(), MouseKeyCallback);
  glfwSetKeyCallback(m_Application->GetWindow()->GetGLFWwindow(), KeyboardKeyCallback);
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (m_Application->GetCamera()->GetShouldMove()) {
    m_Application->GetCamera()->MouseCallback(m_FirstMouse, m_LastMouseWidth, m_LastMouseHeight, xpos, ypos);
  }
}

void Input::SetCursorCanDisappear(bool should) noexcept
{
  m_CursorCanDisappear = should;
  if (!should) {
    SetCursorAppear(true);
  }
}

void Input::SetCursorAppear(bool appear) noexcept
{
  m_CursorShouldAppear = appear;
  uint64_t cursor = appear ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
  glfwSetInputMode(m_Application->GetWindow()->GetGLFWwindow(), GLFW_CURSOR, cursor);
}

void Input::SetCameraCursorToWindowState(bool state)
{
  if (state) {
    m_CursorCanDisappear = false;
    m_SilentInputHandling = true;
    SetCursorAppear(true);
    m_Application->GetCamera()->SetShouldMove(false);
  } else {
    m_CursorCanDisappear = true;
    m_SilentInputHandling = false;
    SetCursorAppear(true);
    m_Application->GetCamera()->SetShouldMove(false);
  }
}

const inline bool Input::GetKeyPressed(int key) noexcept
{
  return glfwGetKey(m_Application->GetWindow()->GetGLFWwindow(), key) == GLFW_PRESS;
}

void Input::ProcessInputRender(Window* window, float delta)
{
  m_FramesCount++;
  Yeager::Interface* intr = m_Application->GetInterface();
  Yeager::BaseCamera* camera = m_Application->GetCamera();
  if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {

    if (!intr->GetExitProgramWindowOpen()) {
      intr->SetExitProgramWindowOpen(true);
    }
  }
  if (intr->GetUserChoiceExitProgram()) {
    glfwSetWindowShouldClose(window->GetGLFWwindow(), true);
  }

  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR && camera->GetShouldMove()) {
    if (GetKeyPressed(GLFW_KEY_W)) {
      camera->UpdatePosition(YgCameraPosition::eCAMERA_FORWARD, delta);
    }
    if (GetKeyPressed(GLFW_KEY_D)) {
      camera->UpdatePosition(YgCameraPosition::eCAMERA_RIGHT, delta);
    }
    if (GetKeyPressed(GLFW_KEY_A)) {
      camera->UpdatePosition(YgCameraPosition::eCAMERA_LEFT, delta);
    }
    if (GetKeyPressed(GLFW_KEY_S)) {
      camera->UpdatePosition(YgCameraPosition::eCAMERA_BACKWARD, delta);
    }
  }
  if (m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) {

    if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      if (glfwGetKey(window->GetGLFWwindow(), GLFW_KEY_F) == GLFW_PRESS) {
        m_Application->GetInterface()->MakeScreenShotAppear(true);
      }
    }
  }
}
void Input::RestoreCameraCursorLastState() noexcept
{
  SetCursorAppear(m_LastState.CursorShouldAppear);
  m_Application->GetCamera()->SetShouldMove(m_LastState.CameraShouldMove);
  m_CursorCanDisappear = m_LastState.CursorCanDissapear;
}
void Input::WriteCameraCursorLastState() noexcept
{
  m_LastState.CursorShouldAppear = m_CursorShouldAppear;
  m_LastState.CameraShouldMove = m_Application->GetCamera()->GetShouldMove();
  m_LastState.CursorCanDissapear = m_CursorCanDisappear;
}