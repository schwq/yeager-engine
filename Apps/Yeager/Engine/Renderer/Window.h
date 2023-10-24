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
#include "../../Common/Utilities.h"

class Window {
 public:
  Window(yg_uint window_x, yg_uint window_y, GLFWerrorfun error, GLFWcursorposfun cursor);
  ~Window();
  YEAGER_NODISCARD GLFWwindow* getWindow() { return m_window; }
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  void LaunchEditor();

 private:
  GLFWwindow* m_window = nullptr;
};
