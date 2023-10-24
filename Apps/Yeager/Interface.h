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

#include "Application.h"
#include "Common/Common.h"
#include "Engine/Editor/Explorer.h"
#include "Engine/Media/AudioHandle.h"
#include "Engine/Renderer/Geometry.h"
#include "Engine/Renderer/TextureHandle.h"
#include "Engine/Renderer/Window.h"

// Simple way to define ImGuiWindowFlags without having to repeat
#define kWindowStatic ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
#define kWindowMoveable ImGuiWindowFlags_None

struct ExplorerObject;

enum class InterfaceMode { kEditorMode, kLauncherMode, kSettingsMode, kAwaitMode, kErrorMode, kCeaseMode };

struct InterfaceWarningWindow {
  yg_string warning;
  yg_uint size_x, size_y;
  bool active = false;
  bool first_log_warning = true;
  bool RenderWarning();
};

struct InterfaceButton {
  InterfaceButton(yg_string name, yg_string text) : m_text(text), m_name(name){};
  bool CenteredButton();
  bool AddButton();
  float TextWidth();
  yg_string m_text;
  yg_string m_name;
};

struct InterfaceImage {
  InterfaceImage(yg_cchar path, Application* m_app);
  void LoadInterfaceImage();
  void LoadInterfaceCenteredImage();
  int m_image_width = 0;
  int m_image_height = 0;
  GLuint m_image_texture = 0;
};

class Interface {
 public:
  Interface(Window* window, Application* app);
  ~Interface();

  bool getInitStatus() { return m_initialize; }
  void RenderUI();
  InterfaceMode GetCurrentMode() { return m_current_mode; }
  const void SetCurrentMode(InterfaceMode mode) { m_current_mode = mode; }
  void CenteredWindow(yg_uint size_x, yg_uint size_y);
  void DisplayWarningWindow();
  void AddWarningWindow(const yg_string& warning, yg_uint size_x = 400, yg_uint size_y = 100);

 private:
  bool m_initialize = false;
  bool m_dont_move_windows_editor = false;
  bool m_comment_window_open = false;

  float size_pixels = 11.0f;
  static yg_uint m_frames;

  InterfaceMode m_current_mode = InterfaceMode::kLauncherMode;
  Application* m_app;
  yg_string comment;
  InterfaceWarningWindow m_current_warning;

  void LaunchImGui(Window* window);
  void DrawExplorer();
  void DrawToolbox();
  void DrawEditorMenu();

  void RenderAwait();
  void RenderLauncher();
  void RenderEditor();
  void RenderSettings(){};
  void RenderError(){};

  void logButton(InterfaceButton button);
  void StyleImGui();
  void CenteredText(yg_string text);
  void AlignForWidth(float width, float alignment = 0.5f);
  void CreateSpaceX(yg_uint count);
  void RenderDebugger();
};
