#include "Application.h"
using namespace Yeager;

ApplicationCore::ApplicationCore()
{
  Log(INFO, "Application running");
}

void ApplicationCore::Setup(ApplicationCoreSetup setup)
{
  m_interface = setup.m_interface;
  m_input = setup.m_input;
  m_window = setup.m_window;
  m_explorer = setup.m_explorer;
  m_camera = setup.m_camera;
  m_scene = setup.m_scene;
  m_renderer = setup.m_renderer;
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  Log(INFO, "Application shutdown");
}

Interface* ApplicationCore::GetInterface()
{
  return m_interface.get();
}
Input* ApplicationCore::GetInput()
{
  return m_input.get();
}
Window* ApplicationCore::GetWindow()
{
  return m_window.get();
}
EditorExplorer* ApplicationCore::GetExplorer()
{
  return m_explorer.get();
}
EditorCamera* ApplicationCore::GetCamera()
{
  return m_camera.get();
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_scene.get();
}
RendererEngine* ApplicationCore::GetRenderer()
{
  return m_renderer.get();
}

ApplicationMode ApplicationCore::GetMode() noexcept
{
  return m_mode;
}
ApplicationState ApplicationCore::GetState() noexcept
{
  return m_state;
}

void ApplicationCore::SetMode(ApplicationMode mode) noexcept
{
  m_mode = mode;
}
void ApplicationCore::SetState(ApplicationState state) noexcept
{
  m_state = state;
}