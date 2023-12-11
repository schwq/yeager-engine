#include "Launcher.h"
#include "Application.h"
using namespace Yeager;
using namespace ImGui;

Launcher::Launcher(unsigned int width, unsigned int height, YgString title, ApplicationCore* app) : m_Application(app)
{
  m_WindowTitle = title;
  Yeager::Log(INFO, "Launching the Engine Launcher");
}

void Launcher::Render()
{
  while ((!UserHasSelect() || m_Application->GetMode() == AppEditor) && m_Application->ShouldRender()) {
    m_Application->GetInterface()->InitRenderFrame();
    glfwPollEvents();
    m_Application->SetMode(Yeager::AppLauncher);
    m_Application->GetInterface()->RenderUI(this);
    m_Application->GetInput()->ProcessInputRender(m_Application->GetWindow(), 1.0f);
    m_Application->GetInterface()->TerminateRenderFrame();
    glfwSwapBuffers(m_Application->GetWindow()->getWindow());
  }
}
void Launcher::BuildNewProject(const Yeager::LauncherProjectPicker& project)
{
  m_ProjectCurrentSelected.m_Name = project.m_Name;
  m_ProjectCurrentSelected.m_ProjectPath = project.m_ProjectPath;
  m_ProjectCurrentSelected.m_SceneRenderer = project.m_SceneRenderer;
  m_ProjectCurrentSelected.m_SceneType = project.m_SceneType;
  m_ProjectCurrentSelected.m_SceneConfigPath = project.m_SceneConfigPath;
  m_ProjectCurrentSelected.m_PlataformTarget = project.m_PlataformTarget;
  m_ProjectCurrentSelected.UserWantToExit = project.UserWantToExit;
}