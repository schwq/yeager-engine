//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
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
#include "Engine/Editor/Camera.h"
#include "Engine/Editor/Explorer.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Renderer/Window.h"
#include "InputHandle.h"
#include "Launcher.h"
#include "Scene.h"

namespace Yeager {

/// @brief Represent the current state for the Application, if its running, paused, crashed ect
enum ApplicationState { AppRunning, AppStopped, AppCrashed };
/// @brief Represent the current mode of the Application, used most for control the current window to appear to the user
enum ApplicationMode { AppEditor, AppLauncher, AppSettings, AppGame, AppLoading };

struct WorldCharacterMatrices {
  YgMatrix4 Projection;
  YgMatrix4 View;
  YgVector3 ViewerPos;
};

///  @brief Class that handles the program
///  @todo Make the EngineCore to handle launcher and editor
class ApplicationCore {
 public:
  ApplicationCore();
  ~ApplicationCore();
  ApplicationCore(ApplicationCore&&) = delete;

  /// @brief Setup the current application
  void Setup();

  /// @brief Get a boolean if the application or user have request for stop rendering, most likely to close the program
  /// @return True if requested, false if not
  bool ShouldRender();

  LauncherProjectPicker RequestLauncher();
  void Render();
  void Terminate() {}

  Interface* GetInterface();
  Input* GetInput();
  Window* GetWindow();
  EditorExplorer* GetExplorer();
  EditorCamera* GetCamera();
  Yeager::Scene* GetScene();

  ApplicationMode GetMode() noexcept;
  ApplicationState GetState() noexcept;

  void SetMode(ApplicationMode mode) noexcept;
  void SetState(ApplicationState state) noexcept;
  void CheckGLAD();

 private:
  void ManifestShaderProps(Yeager::Shader* shader);
  void OpenGLFunc();
  void OpenGLClear();

  void BuildApplicationCoreCompoments();
  YgString RequestWindowEngineName(const LauncherProjectPicker& project);

  float m_DeltaTime = 0.0f;
  float m_LastFrame = 0.0f;
  long long m_FrameCurrentCount = 0;

  void UpdateDeltaTime();
  WorldCharacterMatrices m_WorldMatrices;
  void UpdateWorldMatrices();

  void UpdateListenerPosition();

  Interface* m_Interface;
  Input* m_Input;
  Window* m_Window;
  EditorExplorer* m_EditorExplorer;
  EditorCamera* m_EditorCamera;
  Yeager::Scene* m_Scene;
  Yeager::Launcher* m_Launcher;

  ApplicationState m_state = AppRunning;
  ApplicationMode m_mode = AppLauncher;
};
}  // namespace Yeager