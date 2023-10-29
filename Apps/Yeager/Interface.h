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
// default values for dark style editor
struct ColorschemeConfig {
  float WindowRounding = 5.3f;
  float FrameRounding = 2.3f;
  float ScrollbarRounding = 3;
  ImVec4 Colors_ImGuiCol_Text = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
  ImVec4 Colors_ImGuiCol_TextDisabled = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  ImVec4 Colors_ImGuiCol_WindowBg = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
  ImVec4 Colors_ImGuiCol_ChildBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  ImVec4 Colors_ImGuiCol_PopupBg = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
  ImVec4 Colors_ImGuiCol_Border = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
  ImVec4 Colors_ImGuiCol_BorderShadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  ImVec4 Colors_ImGuiCol_FrameBg = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
  ImVec4 Colors_ImGuiCol_FrameBgHovered = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
  ImVec4 Colors_ImGuiCol_FrameBgActive = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
  ImVec4 Colors_ImGuiCol_TitleBg = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
  ImVec4 Colors_ImGuiCol_TitleBgCollapsed = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
  ImVec4 Colors_ImGuiCol_TitleBgActive = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
  ImVec4 Colors_ImGuiCol_MenuBarBg = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
  ImVec4 Colors_ImGuiCol_ScrollbarBg = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrab = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrabHovered = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
  ImVec4 Colors_ImGuiCol_ScrollbarGrabActive = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
  ImVec4 Colors_ImGuiCol_CheckMark = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
  ImVec4 Colors_ImGuiCol_SliderGrab = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
  ImVec4 Colors_ImGuiCol_SliderGrabActive = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
  ImVec4 Colors_ImGuiCol_Button = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
  ImVec4 Colors_ImGuiCol_ButtonHovered = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
  ImVec4 Colors_ImGuiCol_ButtonActive = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
  ImVec4 Colors_ImGuiCol_Header = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
  ImVec4 Colors_ImGuiCol_HeaderHovered = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
  ImVec4 Colors_ImGuiCol_HeaderActive = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
  ImVec4 Colors_ImGuiCol_ResizeGrip = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
  ImVec4 Colors_ImGuiCol_ResizeGripHovered = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
  ImVec4 Colors_ImGuiCol_ResizeGripActive = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
  ImVec4 Colors_ImGuiCol_PlotLines = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotLinesHovered = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotHistogram = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_PlotHistogramHovered = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  ImVec4 Colors_ImGuiCol_TextSelectedBg = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
};

enum class InterfaceMode { kEditorMode, kLauncherMode, kSettingsMode, kAwaitMode, kErrorMode, kCeaseMode };

struct InterfaceWarningWindow {
  yg_string warning;
  yg_uint size_x, size_y;
  bool active = false;
  bool first_log_warning = true;
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
  void ApplyColorscheme(ColorschemeConfig colorscheme)
  {
    m_colorscheme = colorscheme;
    LoadColorscheme();
  }

 private:
  bool m_initialize = false;
  bool m_dont_move_windows_editor = false;
  bool m_comment_window_open = false;

  float size_pixels = 11.0f;
  static yg_uint m_frames;

  ColorschemeConfig m_colorscheme;
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
  void LoadColorscheme();
  void CenteredText(yg_string text);
  void AlignForWidth(float width, float alignment = 0.5f);
  void CreateSpaceX(yg_uint count);
  void RenderDebugger();
};
