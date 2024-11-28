//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Editor/UI/Interface.h"
#include "Main/IO/InputHandle.h"
#include "Main/Scene/Scene.h"
#include "Window.h"

namespace Yeager {

class ApplicationCore;

struct LauncherProjectPicker {
  LauncherProjectPicker() {}
  LauncherProjectPicker(const OpenProjectsDisplay& other);
  void ConstructorFrom(const OpenProjectsDisplay& other);
  String m_Name = "Default";
  String m_AuthorName = "Default";
  String m_ProjectFolderPath = "Default";
  String m_ProjectConfigurationPath = "Default";
  String m_PlataformTarget = "Universal";
  TimePointType m_ProjectDateOfCreation;
  Yeager::SceneType m_SceneType;
  Yeager::SceneRenderer m_SceneRenderer;
  bool UserWantToExit = false;
  std::optional<TemplateHandle> m_TemplateHandle;
};

class Launcher {
 public:
  Launcher(Uint width, Uint height, String title, ApplicationCore* app);

  void Render();
  void BuildNewProject(const Yeager::LauncherProjectPicker& project);

  LauncherProjectPicker GetSelectedProject() noexcept { return m_ProjectCurrentSelected; }
  LauncherProjectPicker* GetCurrentProjectPicked() noexcept { return &m_ProjectCurrentSelected; }
  Yeager::Scene* GetNewProjectScenePtr() noexcept { return m_NewProjectScene; }

  YEAGER_CONSTEXPR LauncherProjectPicker* GetSelectedProjectPtr() noexcept { return &m_ProjectCurrentSelected; }
  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE bool UserHasSelect() noexcept { return m_UserDoneSelecting; }
  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE bool GetNewProjectLoaded() noexcept { return m_IsNewProjectLoaded; }
  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE void SetUserHasSelect(bool selected) noexcept { m_UserDoneSelecting = selected; }
  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE void SetNewProjectLoaded(bool loaded) noexcept { m_IsNewProjectLoaded = loaded; }

 private:
  bool m_UserDoneSelecting = false;
  bool m_IsNewProjectLoaded = false;
  String m_WindowTitle;
  std::vector<LauncherProjectPicker> m_ProjectsInLauncher;
  LauncherProjectPicker m_ProjectCurrentSelected;
  Yeager::Scene* m_NewProjectScene = YEAGER_NULLPTR;
  ApplicationCore* m_Application = YEAGER_NULLPTR;
};
}  // namespace Yeager