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
#include "Engine/Renderer/ImageUtilities.h"
#include "Engine/Renderer/Window.h"

#define YEAGER_KEY_PRESS 1
#define YEAGER_KEY_RELEASE 2
#define YEAGER_KEY_PRESS_AND_RELEASE 3

namespace Yeager {
class ApplicationCore;

struct KeyMap {
  KeyMap(uint64_t GlfwMacro) { this->GlfwMacro = GlfwMacro; }
  constexpr KeyMap(Yeager::KeyMap&& other)
  {
    this->GlfwMacro = other.GlfwMacro;
    this->state = other.state;
    this->action = other.action;
  }
  constexpr KeyMap(const Yeager::KeyMap& other)
  {
    this->GlfwMacro = other.GlfwMacro;
    this->state = other.state;
    this->action = other.action;
  }
  uint64_t GlfwMacro = GLFW_KEY_UNKNOWN;
  uint8_t state = GLFW_RELEASE;
  bool action = false;
  bool AddStateAwaitAction(uint8_t await)
  {
    if (await == GLFW_PRESS && state == GLFW_RELEASE) {
      state = await;
      action = false;
      return false;
    }

    if (await == GLFW_RELEASE && state == GLFW_PRESS) {
      state = await;
      action = true;
      return true;
    }

    state = GLFW_RELEASE;
    action = false;
    return false;
  }
};

/** 
 * TODO Make this a std::map (im having some confusion on this topic)
 */

extern std::vector<KeyMap> KeyMapping;
extern KeyMap* FindKeyMap(uint8_t key);

/// @brief Stores the last state of the camera and cursor before some modification
struct CameraCursorLastState {
  bool CursorShouldAppear = false;
  bool CameraShouldMove = false;
  bool CursorCanDissapear = false;
  const inline bool CursorWasDissapearing() noexcept { return CursorCanDissapear; }
  const inline bool WasMoving() noexcept { return CursorShouldAppear && CameraShouldMove; }
};

/// @brief Class that handles all keyboard and mouse inputs and process them into the respective requests
class Input {
 public:
  /// @brief Constructor of the Input, assign the ApplicationCore pointer given to the class
  /// @param app The current application pointer
  Input(Yeager::ApplicationCore* app);
  ~Input();
  Input() {}

  void InitializeCallbacks();
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
  void SetCursorCanDisappear(bool should) noexcept;
  static void SetCursorAppear(bool appear) noexcept;

  /// @brief When a window popup in the engine editor, we dont want the camera moving around, and the cursor not appearing, so this function is set to true
  ///  when the window is showing, and false when is closed
  void SetCameraCursorToWindowState(bool state);

  /// @brief Returns a boolean representing if the GLFW_KEY (key) have been pressed
  /// @param key The GLFW macro representing the key, something like GLFW_KEY_X (heres X is the key)
  /// @return True if the key have been pressed, false if not
  const inline bool GetKeyPressed(int key) noexcept;

  void RestoreCameraCursorLastState() noexcept;

  void WriteCameraCursorLastState() noexcept;

 private:
  static void MouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
  static void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

  static CameraCursorLastState m_LastState;
  static Yeager::ApplicationCore* m_Application;
  /// @brief The last y mouse position
  static float m_LastMouseHeight;
  /// @brief The last x mouse position
  static float m_LastMouseWidth;
  /// @brief  Used for handling the first time the mouse is called, and the camera makes a quickly weird movement, kinda annoying
  static bool m_FirstMouse;
  static unsigned int m_FramesCount;
  bool m_CursorCanDisappear = true;
  static bool m_CursorShouldAppear;
  int cursor_x = 0, cursor_y = 0;
};
}  // namespace Yeager