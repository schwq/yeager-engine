//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023-present
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

static void glfwErrorCallback(int error, yg_cchar description)
{
  VLOG_F(ERROR, description);
}

class Window {
 public:
  Window(yg_uint window_x, yg_uint window_y, GLFWcursorposfun cursor);
  ~Window();
  YEAGER_NODISCARD GLFWwindow* getWindow() { return m_window; }
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void WindowResizeCallback(GLFWwindow* window, int width, int height);
  static void HandleError(int code, const char* description);
  void GetWindowSize(int* width, int* height);

 private:
  GLFWwindow* m_window = nullptr;
  GLFWmonitor* m_primary_monitor = nullptr;
  const GLFWvidmode* m_vid_mode;
  yg_uint m_window_monitor_x = 0, m_window_monitor_y = 0;
};
