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
#include "Engine/Editor/Camera.h"
#include "Engine/Renderer/Window.h"

namespace Yeager {
class ApplicationCore;
}
/// @brief Class that handles all keyboard and mouse inputs and process them into the respective requests
class Input {
 public:
  /// @brief Constructor of the Input, assign the ApplicationCore pointer given to the class
  /// @param app The current application pointer
  Input(Yeager::ApplicationCore* app);
  ~Input();

  /// @brief Process all input registed by the GLFW library loaded
  /// @param window The current GLFW window been used
  /// @param delta Delta time for handling delays
  void ProcessInputRender(Window* window, float delta);

  /// @brief GLFW mouse callback, this is called everytime the mouse is moved, and register the current position
  /// @param window The current GLFW window been used
  /// @param xpos The X position of the mouse
  /// @param ypos The Y position of the mouse
  static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

  /// @brief Sets if the cursor can disappear in the current state, like when moving around the settings windows, the cursor should not disappear
  /// @param should True if the mouse can disappear, false if not
  const void SetCursorCanDisappear(bool should);

  /// @brief Returns a boolean representing if the GLFW_KEY (key) have been pressed
  /// @param key The GLFW macro representing the key, something like GLFW_KEY_X (heres X is the key)
  /// @return True if the key have been pressed, false if not
  bool GetKeyPressed(int key);

  /// @brief Went set to true, the cursor is forced to appear, and does dissapear until the function is set to false after
  /// @param make True if the cursor should appear, false if not
  void MakeCursorStaticAppear(bool make);

 private:
  static Yeager::ApplicationCore* m_app;
  /// @brief The last y mouse position
  static float lastY;
  /// @brief The last x mouse position
  static float lastX;
  /// @brief  Used for handling the first time the mouse is called, and the camera makes a quickly weird movement, kinda annoying
  static bool firstMouse;
  static yg_uint m_framesCount;
  bool m_cursor_can_disappear = true;
};