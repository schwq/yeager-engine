//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Renderer/Window.h"
#include "InputHandle.h"
#include "Scene.h"

namespace Yeager {

class ApplicationCore;

struct LauncherProjectPicker {
  String m_Name = "Default";
  String m_AuthorName = "Default";
  String m_ProjectFolderPath = "Default";
  String m_ProjectConfigurationPath = "Default";
  String m_PlataformTarget = "Universal";
  YgTime_t m_ProjectDateOfCreation;
  Yeager::SceneType m_SceneType;
  Yeager::SceneRenderer m_SceneRenderer;
  bool UserWantToExit = false;
};

class Launcher {
 public:
  Launcher(unsigned int width, unsigned int height, String title, ApplicationCore* app);

  void Render();
  LauncherProjectPicker GetSelectedProject() noexcept { return m_ProjectCurrentSelected; }
  constexpr LauncherProjectPicker* GetSelectedProjectPtr() noexcept { return &m_ProjectCurrentSelected; }

  constexpr inline bool UserHasSelect() noexcept { return m_UserDoneSelecting; }
  constexpr inline void SetUserHasSelect(bool selected) noexcept { m_UserDoneSelecting = selected; }
  LauncherProjectPicker* GetCurrentProjectPicked() noexcept { return &m_ProjectCurrentSelected; }
  Yeager::Scene* GetNewProjectScenePtr() noexcept { return m_NewProjectScene; }
  constexpr inline void SetNewProjectLoaded(bool loaded) noexcept { m_IsNewProjectLoaded = loaded; }
  constexpr inline bool GetNewProjectLaoded() noexcept { return m_IsNewProjectLoaded; }

  void BuildNewProject(const Yeager::LauncherProjectPicker& project);

 private:
  String m_WindowTitle;
  std::vector<LauncherProjectPicker> m_ProjectsInLauncher;
  LauncherProjectPicker m_ProjectCurrentSelected;
  bool m_IsNewProjectLoaded = false;
  Yeager::Scene* m_NewProjectScene = YEAGER_NULLPTR;
  ApplicationCore* m_Application;
  bool m_UserDoneSelecting = false;
};
}  // namespace Yeager