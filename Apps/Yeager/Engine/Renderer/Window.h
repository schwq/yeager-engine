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

#define YEAGER_EDITOR_DEFAULT_TITLE "Yeager Editor"
#define YEAGER_LAUNCHER_DEFAULT_TITLE "Yeager Launcher"
#define YEAGER_GENERATE_LAUNCHER_WINDOW 0x01
#define YEAGER_GENERATE_EDITOR_WINDOW 0x02

namespace Yeager {

class ApplicationCore;

struct AspectRatio {
  enum Enum { eASPECT_21_9, eASPECT_4_3 };
  YEAGER_ENUM_TO_STRING(AspectRatio)
  YEAGER_STRING_TO_ENUM(AspectRatio)
  static float ToValue(Enum type);
};

struct OnScreenSpaceRange {
  Uint Width = 0, Height = 0;
  Uint PositionX = 0, PositionY = 0;
};

struct WindowInfo {
  Vector2 EditorSize = Vector2(0.0f, 0.0f);
  Vector2 LauncherSize = Vector2(0.0f, 0.0f);
  Vector2 FrameBufferSize = Vector2(0.0f, 0.0f);
  Vector2 FrameBufferPosition = Vector2(0.0f, 0.0f);
  String EditorTitle = YEAGER_EDITOR_DEFAULT_TITLE;
  String LauncherTitle = YEAGER_LAUNCHER_DEFAULT_TITLE;
  GLFWcursorposfun CursorFunc;
};

/* Used when we want to regenerate the window with different hints, we change this struct and the window is created based on it*/
struct WindowCreationHints {
  Uint ContextVersionMajor = 3;
  Uint ContextVersionMinor = 3;
  uint32_t OpenGLProfile = GLFW_OPENGL_CORE_PROFILE;
  Uint AntiAliasingSamples = 4;
#ifdef YEAGER_SYSTEM_MACOS
  bool OpenGLForwandCompat = GL_TRUE;
#endif
};

class Window {
 public:
  Window(Yeager::ApplicationCore* application);
  ~Window();

  void GenerateWindow(String title, GLFWcursorposfun cursor, DFlags wndType);

  YEAGER_NODISCARD GLFWwindow* GetGLFWwindow() { return m_WindowHandle; }
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void HandleError(int code, Cchar description);
  void GetWindowSize(int* width, int* height);
  Vector2 GetWindowSize() const;
  bool GenerateWindow(unsigned window_x, Uint window_y, String title);
  bool RegenerateMainWindow(Uint window_x, Uint window_y, String title, GLFWcursorposfun cursor) noexcept;

  /* Glfw3 Callbacks  */
  static void ResizeCallback(GLFWwindow* window, int width, int height);
  static void WindowMaximizeCallback(GLFWwindow* window, int maximized);

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
  static WindowInfo m_WindowInformation;
  WindowCreationHints m_WindowHints;
  GLFWwindow* m_WindowHandle = YEAGER_NULLPTR;
  unsigned char* m_IconImageData = YEAGER_NULLPTR;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  bool m_Maximized = false, m_FullScreen = false;
};
}  // namespace Yeager