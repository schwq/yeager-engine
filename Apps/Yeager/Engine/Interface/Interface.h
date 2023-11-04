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

#include "../../Common/Common.h"
#include "../Editor/Explorer.h"
#include "../Media/AudioHandle.h"
#include "../Renderer/Geometry.h"
#include "../Renderer/TextureHandle.h"
#include "../Renderer/Window.h"
#include "IconsFontAwesome6.h"
#include "Notification.h"
#include "UIManagement.h"

namespace Yeager {
class ApplicationCore;
}  // namespace Yeager

/// @brief Struct of the Imgui Colorscheme, the defaults values are for the dark mode,
/// @brief colorschemes can been readed from the colorscheme configuration folder
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

/// @brief  Struct of the Warning window, its takes a string for the message, x and y sizes,
/// @brief  its also have two booleans, active for holding just one warning at the time, and first log
/// @brief  so it doesnt keep logging the same thing
struct InterfaceWarningWindow {
  yg_string warning;
  yg_uint size_x, size_y;
  bool active = false;
  bool first_log_warning = true;
};

/// @brief Struct representing a Imgui Button, used for logging and debugging the pressed buttons
struct InterfaceButton {
  InterfaceButton(yg_string name, yg_string text) : m_text(text), m_name(name){};
  /// @brief Centered a button in the ImGui Window
  /// @return True if the button have been clicked
  bool CenteredButton();
  /// @brief Adds a button to the screen, this is the same as the ImGui one, but is from the custom struct
  /// @return True if the button have been clicked
  bool AddButton();
  /// @brief Calculate the button label text width
  /// @return The text width in float notation
  float TextWidth();
  yg_string m_text;
  yg_string m_name;
};

/// @brief  Holds the Imgui Image, and makes quick loading, without having to make the same boring process, over and over
struct InterfaceImage {
  InterfaceImage(yg_cchar path);
  void LoadInterfaceImage();
  void LoadInterfaceCenteredImage();
  int m_image_width = 0;
  int m_image_height = 0;
  GLuint m_image_texture = 0;
};
/// @brief Class that holds the interface management, use ImGui as library, the current program must be running just one instance of this class
/// @attention In the future, we need to write a self-custom GUI library in C for this Engine
/// @attention so we can have more control over the drawing methods and increase performance
class Interface {
 public:
  /// @brief Interface Constructor, loads fonts and the colorscheme readed from the configuration file
  /// @param window The GLFW window pointer the program is current using
  /// @param app The ApplicationCore pointer for management
  Interface(Window* window, Yeager::ApplicationCore* app);
  ~Interface();

  /// @brief Return a boolean representing if the program have been initialize with success
  /// @return True if initialize, false if not
  bool getInitStatus() { return m_initialize; }

  /// @brief The main function to be called, it check switch current mode the program is at, and pass to the correct function for the GUI rendering
  void RenderUI();

  /// @brief ImGui have a function that let the user choice the next window position, this function does the same
  /// @param size_x Window width
  /// @param size_y Window height
  void CenteredWindow(yg_uint size_x, yg_uint size_y);
  /// @brief Displays the current warning window stored in the Interface class
  void DisplayWarningWindow();
  /// @brief  Add a warning window to the Interface class, and waits to be display when the DisplayWarningWindow is called
  void AddWarningWindow(const yg_string& warning, yg_uint size_x = 400, yg_uint size_y = 100);
  /// @brief Self explain, apply the colorscheme given to the interface UI by calling LoadColorscheme
  /// @param colorscheme The colorscheme to be apply
  void ApplyColorscheme(ColorschemeConfig colorscheme)
  {
    m_colorscheme = colorscheme;
    LoadColorscheme();
  }
  /// @brief Set a boolean value to the current interface, representing if the Exit Program window need to the rendered to the screen
  /// @param exit Boolean representing if the window should render
  void SetExitProgramWindowOpen(bool exit) { m_exit_program_window_open = exit; }
  /// @brief Get a boolean value representing if the current interface have the Exit Program window set to be rendered or already been render
  /// @return True if the window is set to render, false if not
  bool GetExitProgramWindowOpen() { return m_exit_program_window_open; }
  /// @brief In the Exit Program window, the user can choice to cancel the operation or exit, this function return a boolean representing if the user choice to exit the program or not
  /// @return True if the user set to Exit the Program, false if not
  bool GetUserChoiceExitProgram() { return m_user_exit_program; }
  /// @brief Display the Exit Program Window
  /// @return The user choice to exit the program or not
  bool WindowExitProgram();

 private:
  bool m_initialize = false;
  bool m_dont_move_windows_editor = false;
  bool m_comment_window_open = false;
  bool m_exit_program_window_open = false;
  bool m_user_exit_program = false;

  float size_pixels = 14.0f;
  static yg_uint m_frames;
  ImVec2 m_menu_bar_size = ImVec2(0, 0);

  Yeager::ApplicationCore* m_app;
  ColorschemeConfig m_colorscheme;
  yg_string comment;
  InterfaceWarningWindow m_current_warning;
  Yeager::InterfaceWindow m_console_window;
  Yeager::InterfaceWindow m_toolbox_window;
  void NewProjectWindow();

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
