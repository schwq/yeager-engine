#include "Application.h"
#include "Common/LogEngine.h"
#include "InputHandle.h"

Application::Application()
{
  Yeager::Log(INFO, kSystem, "Start program");
}

Application::~Application() {}

bool Application::EnterKeyPressed()
{
  return glfwGetKey(m_window->getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS;
}

bool Application::ShouldRendererActive()
{
  return (glfwWindowShouldClose(m_window->getWindow())) ? false : true;
}

void Application::SetupApplication(ApplicationSetup setup)
{
  m_camera = setup.ptr_camera;
  m_window = setup.ptr_window;
  m_engine = setup.ptr_engine;
  m_input = setup.ptr_input;
  m_interface = setup.ptr_interface;
  m_explorer = setup.ptr_explorer;
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
    case ApplicationCurrentMode::kCeaseMode:
      m_input->SetCurrentMode(InputCurrentMode::kCeaseMode);
      m_interface->SetCurrentMode(InterfaceMode::kCeaseMode);
  }
}
