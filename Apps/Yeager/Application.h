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
// clang-format off
#include "Common/Common.h"
#include "Common/Utilities.h"
#include "Engine/Renderer/Entity.h"
#include "Engine/Renderer/TextureHandle.h"
#include "Engine/Editor/ToolboxObj.h"
#include "Engine/Editor/Camera.h"
#include "Common/LogEngine.h"
#include "Engine/Editor/Explorer.h"
#include "Engine/Renderer/ImportedObj.h"
#include "Engine/Renderer/Render.h"
#include "Engine/Renderer/Window.h"
#include "InputHandle.h"
#include "Interface.h"

class RendererEngine;
class Window;
class Input;
class Interface;
class EditorCamera;
class EditorExplorer;
class EditorConsole;
class ImportedObject;
class Yeager::GameEntity;
struct ConsoleMessageType;

// clang-format on

struct ApplicationSetup {
  Input* ptr_input;
  Window* ptr_window;
  RendererEngine* ptr_engine;
  Interface* ptr_interface;
  EditorCamera* ptr_camera;
  EditorExplorer* ptr_explorer;
};

enum class ApplicationCurrentMode {
  kEditorMode = 0,
  kErrorMode = 1,
  kAwaitMode = 2,
  kSettingsMode = 3,
  kLauncherMode = 4,
  kCeaseMode = 5
};

class Application {
 public:
  Application();
  ~Application();

  const void SetCurrentMode(ApplicationCurrentMode mode);

  constexpr Window* GetWindowManager() const { return m_window; }
  constexpr RendererEngine* GetRendererEngine() const { return m_engine; }
  constexpr Input* GetInput() const { return m_input; }
  constexpr Interface* GetInterface() const { return m_interface; }
  constexpr ApplicationCurrentMode GetCurrentMode() const { return m_current_mode; }
  constexpr EditorCamera* GetEditorCamera() const { return m_camera; }
  void SetupApplication(ApplicationSetup setup);
  bool ShouldRendererActive();
  bool EnterKeyPressed();

  constexpr EditorExplorer* GetExplorer() const { return m_explorer; }
  constexpr std::vector<Yeager::Texture2D>* GetVectorUserTexture2D() { return &m_textures_2d_user_created; }
  constexpr std::vector<ImportedObject>* GetImportedObjects() { return &m_imported_objects; }

 private:
  ApplicationCurrentMode m_current_mode = ApplicationCurrentMode::kEditorMode;
  EditorCamera* m_camera = nullptr;
  Window* m_window = nullptr;
  RendererEngine* m_engine = nullptr;
  Input* m_input = nullptr;
  Interface* m_interface = nullptr;
  EditorConsole* m_console = nullptr;
  EditorExplorer* m_explorer = nullptr;
  std::vector<Yeager::Texture2D> m_textures_2d_user_created;
  std::vector<ImportedObject> m_imported_objects;
};
