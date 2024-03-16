//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/
//
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

namespace Yeager {

struct WindowInfo {
  Vector2 Size = Vector2(0.0f, 0.0f);
  String Title = YEAGER_NULL_LITERAL;
  GLFWcursorposfun CursorFunc;
};

/* Used when we want to regenerate the window with different hints, we change this struct and the window is created based on it*/
struct WindowCreationHints {
  unsigned int ContextVersionMajor = 3;
  unsigned int ContextVersionMinor = 3;
  uint32_t OpenGLProfile = GLFW_OPENGL_CORE_PROFILE;
  unsigned int AntiAliasingSamples = 4;
#ifdef YEAGER_SYSTEM_MACOS
  bool OpenGLForwandCompat = GL_TRUE;
#endif
};

class Window {
 public:
  Window(unsigned int window_x, unsigned int window_y, String title, GLFWcursorposfun cursor);
  ~Window();

  YEAGER_NODISCARD GLFWwindow* getWindow() { return m_WindowHandle; }
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void HandleError(int code, Cchar description);
  void GetWindowSize(int* width, int* height);
  bool GenerateWindow(unsigned window_x, unsigned int window_y, String title);
  bool RegenerateMainWindow(unsigned int window_x, unsigned int window_y, String title,
                            GLFWcursorposfun cursor) noexcept;
  static void ResizeCallback(GLFWwindow* window, int width, int height);
  bool CheckIfOpenGLContext();
  /**
   * @brief Regenerates the main window with the new given number of samples for the anti aliasing (ps: glfw needs to recreate the window to function)
   *  , it returns true if the process was successed, or false if otherwise
   */
  bool ChangeAntiAliasingSamples(int samples);

  YEAGER_FORCE_INLINE bool ValidateAntiAliasingSampleValue(int samples) const
  {
    if (samples == 1 || samples == 2 || samples == 4 || samples == 8)
      return true;
    return false;
  }

  YEAGER_FORCE_INLINE WindowInfo* GetWindowInformationPtr() { return &m_WindowInformation; }
  YEAGER_FORCE_INLINE WindowCreationHints* GetWindowCreationHintsPtr() { return &m_WindowHints; }

  void BuildWindowHints();

 private:
  WindowInfo m_WindowInformation;
  WindowCreationHints m_WindowHints;
  GLFWwindow* m_WindowHandle = YEAGER_NULLPTR;
  unsigned char* m_IconImageData = YEAGER_NULLPTR;
};
}  // namespace Yeager