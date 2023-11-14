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
#include "Scene.h"

namespace Yeager {
/// @brief Represent the current state for the Application, if its running, paused, crashed ect
enum ApplicationState { AppRunning, AppStopped, AppCrashed };
/// @brief Represent the current mode of the Application, used most for control the current window to appear to the user
enum ApplicationMode { AppEditor, AppLauncher, AppSettings, AppGame, AppLoading };

/** @brief Struct to handle the constructor of the application 
 * @param Interface @param Window @param Camera @param Scene @param Renderer
 * @param Input @param Explorer
 */
struct ApplicationCoreSetup {
  std::shared_ptr<Interface> m_interface;
  std::shared_ptr<Input> m_input;
  std::shared_ptr<Window> m_window;
  std::shared_ptr<EditorExplorer> m_explorer;
  std::shared_ptr<EditorCamera> m_camera;
  std::shared_ptr<Yeager::Scene> m_scene;
};

///  @brief Class that handles the program
///  @todo Make the EngineCore to handle launcher and editor
class ApplicationCore {
 public:
  ApplicationCore();
  ~ApplicationCore();
  ApplicationCore(ApplicationCore&&) = delete;

  /// @brief Setup the current application
  /// @param setup An ApplicationSetup already built
  void Setup(ApplicationCoreSetup setup);

  /// @brief Get a boolean if the application or user have request for stop rendering, most likely to close the program
  /// @return True if requested, false if not
  bool ShouldRender();

  void Render();

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
  void ManifestShaderProps(Yeager::Shader* shader, YgMatrix4 view, YgMatrix4 projection, YgVector3 viewPos);

  std::shared_ptr<Interface> m_interface = YEAGER_NULLPTR;
  std::shared_ptr<Input> m_input = YEAGER_NULLPTR;
  std::shared_ptr<Window> m_window = YEAGER_NULLPTR;
  std::shared_ptr<EditorExplorer> m_explorer = YEAGER_NULLPTR;
  std::shared_ptr<EditorCamera> m_camera = YEAGER_NULLPTR;
  std::shared_ptr<Yeager::Scene> m_scene = YEAGER_NULLPTR;

  ApplicationState m_state = AppRunning;
  ApplicationMode m_mode = AppLauncher;
};
}  // namespace Yeager