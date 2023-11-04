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
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "IconsFontAwesome6.h"

// Simple way to define ImGuiWindowFlags without having to repeat
#define kWindowStatic ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
#define kWindowMoveable ImGuiWindowFlags_None

namespace Yeager {

enum class WindowRelativePos {
  /// Children window
  ABOVE,
  UNDER,
  LEFT,
  RIGHT,
  /// Root window
  ABOVE_MIDDLE,
  UNDER_MIDDLE,
  LEFT_MIDDLE,
  RIGHT_MIDDLE,
  LEFT_ABOVE,
  RIGHT_ABOVE,
  LEFT_UNDER,
  RIGHT_UNDER,
  MIDDLE
};

enum EYGWindowRule {
  EWindowFullscreen,
  EWindowFollowAbove,
  EWindowFollowUnder,
  EWindowSameSizeParent,
  EWindowSameWidthParent,
  EWindowSameHeightParent,
  EWindowRoundingCorner
};

struct WindowsRulesContext {
  bool fullscreen = false;
  bool follow_above = false;
  bool follow_under = false;
  bool same_size_parent = false;
  bool same_width_parent = false;
  bool same_height_parent = false;
  bool rounding_corner = false;
};

class InterfaceWindow {
 public:
  /// @brief Constructor for a ImGui interface window, this kind of contructor is for children window, where their position is relative to a parent window (root)
  /// @param title The window title to be display in the menu
  /// @param size The window size, is recommended to have the same width as the parent window
  /// @param parent_window The pointer to the parent window
  /// @param parent_relative_pos You can change the position the children window have from the parent window, like some space between them
  /// @param relative_pos Where the children window will appear in relation to the parent window (above the parent, under, left, and right)
  InterfaceWindow(yg_string title, ImVec2 size, InterfaceWindow* parent_window,
                  ImVec2 parent_relative_pos = ImVec2(0, 0), WindowRelativePos relative_pos = WindowRelativePos::LEFT);
  /// @brief Constructor for a ImGui interface window, this constructor is for a parent window (root), that will have multiple children following it
  /// @param title The window title to be display in the menu
  /// @param size The window size
  /// @param position The window position relative to the screen (GLFW window)
  /// @param follow_glfw_window Boolean if set to true, went the user resize the GLFW window, the ImGui window will follow to stay at the place and dont dissapear or stay in the middle of the screen
  /// @param glfw_relative_pos A position enum that indicate the relative position to the glfw that the root window will follow
  InterfaceWindow(yg_string title, ImVec2 size, ImVec2 position, bool follow_glfw_window = true,
                  WindowRelativePos glfw_relative_pos = WindowRelativePos::RIGHT);
  ~InterfaceWindow();
  InterfaceWindow() {}

  void Begin();
  void End();

  bool GetIsRoot();
  bool GetFollowGLFWWindow();
  /// @brief A boolean representing if the X and Y provided are inside the window space, can be used for when the cursor hover the window
  /// @param x The x position of the element
  /// @param y The y position
  /// @return True if collision have been detected, false if not
  bool GetCollision(yg_uint x, yg_uint y);
  InterfaceWindow* GetParent();
  ImVec2 GetParentRelativePos();
  WindowRelativePos GetRelativePos();
  ImVec2 GetSize();
  ImVec2 GetScreenPos();
  void SetMenuBarSize(ImVec2 size);
  void SetSize(ImVec2 size);
  void SetPos(ImVec2 pos);
  void SetRule(EYGWindowRule rule, bool value);
  bool GetRule(EYGWindowRule rule);

 private:
  void CalcScreenPos();
  /// @brief Some rules must be processed before the window calculation screen is called
  void ProcessPreRules();
  /// @brief Some rules must be processed after the window calculation screen is called
  void ProcessPosRules();

  bool m_root_window = false;
  bool m_follow_glfw_window = true;
  yg_string m_title = "default";
  InterfaceWindow* m_parent_window = nullptr;
  WindowRelativePos m_relative_pos;

  WindowRelativePos m_glfw_relative_pos;
  ImVec2 m_parent_relative_pos;
  ImVec2 m_menu_bar_size = ImVec2(0, 0);
  ImVec2 m_size = ImVec2(100, 100);
  ImVec2 m_screen_position = ImVec2(0.0f, 0.0f);
  ImVec2 temp_resize = ImVec2(0, 0);
  WindowsRulesContext m_rules;
};
}  // namespace Yeager

static inline void StringToWindowWidth(yg_string str, yg_uint window_x, yg_uint window_y);
