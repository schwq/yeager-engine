#include "application.h"
#include "input.h"
#include <GLFW/glfw3.h>

Application::Application() { VLOG_F(INFO, "Start program"); }

Application::~Application() {
  delete explorer;
  for(uint x = 0; x < m_cubes_user_created.size(); x++) {
    delete m_cubes_user_created[x];
  }
  for(uint x = 0; x < m_2d_textures_user_handle.size(); x++) {
    delete m_2d_textures_user_handle[x];
  }
}

bool Application::EnterKeyPressed() {
  return glfwGetKey(m_window->getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS;
}

bool Application::ShouldRendererActive()
{
  if (glfwWindowShouldClose(m_window->getWindow())) {
    return false;
  }
  return true;
}

void Application::SetupApplication(Input *input, Window *window,
                                   RendererEngine *engine, EditorCamera *camera,
                                   Interface *interfaceUI)
{
  m_input = input;
  m_engine = engine;
  m_camera = camera;
  m_window = window;
  m_interface = interfaceUI;
  explorer = new EditorExplorer(this);
}

const void Application::SetCurrentMode(ApplicationCurrentMode mode)
{
  switch (mode) {
    case ApplicationCurrentMode::kEditorMode:
      m_input->SetCurrentMode(InputCurrentMode::kEditorMode);
      m_interface->SetCurrentMode(InterfaceMode::kEditorMode);
      break;
    case ApplicationCurrentMode::kAwaitMode:
      m_input->SetCurrentMode(InputCurrentMode::kAwaitMode);
      m_interface->SetCurrentMode(InterfaceMode::kAwaitMode);
      break;
    case ApplicationCurrentMode::kLauncherMode:
      m_input->SetCurrentMode(InputCurrentMode::kLauncherMode);
      m_interface->SetCurrentMode(InterfaceMode::kLauncherMode);
      break;
    case ApplicationCurrentMode::kSettingsMode:
      m_input->SetCurrentMode(InputCurrentMode::kSettingsMode);
      m_interface->SetCurrentMode(InterfaceMode::kSettingsMode);
      break;
    case ApplicationCurrentMode::kErrorMode:
      m_input->SetCurrentMode(InputCurrentMode::kErrorMode);
      m_interface->SetCurrentMode(InterfaceMode::kErrorMode);
      break;

      // TODO rest of modes
  }
}
