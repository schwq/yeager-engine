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
#include "Engine/Renderer/Window.h"

class Application;

enum class InputCurrentMode { kEditorMode, kCeaseMode, kLauncherMode, kSettingsMode, kErrorMode, kAwaitMode };

class Input {
 public:
  Input(Application* app);
  ~Input();

  void ProcessInputRender(Window* window, float delta);
  static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
  InputCurrentMode GetCurrentMode() { return m_current_mode; }
  const void SetCurrentMode(InputCurrentMode mode) { m_current_mode = mode; };
  const void SetCursorCanDisappear(bool should);

 private:
  InputCurrentMode m_current_mode = InputCurrentMode::kAwaitMode;
  static Application* m_app;
  static float lastY;
  static float lastX;
  static bool firstMouse;
  static yg_uint m_framesCount;
  bool m_cursor_can_disappear = true;
};