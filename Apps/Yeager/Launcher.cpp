#include "Launcher.h"
#include "Application.h"
using namespace Yeager;
using namespace ImGui;

Launcher::Launcher(Uint width, Uint height, String title, ApplicationCore* app)
    : m_Application(app), m_WindowTitle(title)
{
  Yeager::Log(INFO, "Launching the Engine Launcher");
}

void Launcher::Render()
{
  while ((!UserHasSelect() || m_Application->GetMode() == ApplicationMode::eAPPLICATION_EDITOR) &&
         m_Application->ShouldRender()) {
    m_Application->GetInterface()->InitRenderFrame();
    glfwPollEvents();
    m_Application->SetMode(ApplicationMode::eAPPLICATION_LAUNCHER);
    m_Application->GetInterface()->RenderUI(this);
    m_Application->GetInput()->ProcessInputRender(m_Application->GetWindow(), 1.0f);
    m_Application->GetInterface()->TerminateRenderFrame();
    glfwSwapBuffers(m_Application->GetWindow()->GetGLFWwindow());
    m_Application->GetRequestHandle()->HandleRequests();
  }
}
void Launcher::BuildNewProject(const Yeager::LauncherProjectPicker& project)
{
  m_ProjectCurrentSelected = project;
}