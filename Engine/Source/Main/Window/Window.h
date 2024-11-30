//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/
//
//    Copyright (C) 2023 - Present - Present
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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#define YEAGER_EDITOR_DEFAULT_TITLE "Yeager Editor"
#define YEAGER_LAUNCHER_DEFAULT_TITLE "Yeager Launcher"
#define YEAGER_GENERATE_LAUNCHER_WINDOW 0x01
#define YEAGER_GENERATE_EDITOR_WINDOW 0x02

namespace Yeager {

class ApplicationCore;

/**
 * @brief Holds the enum that dictate the aspect ratios the engine accepts and converts them to float values
 */
struct AspectRatio {
  enum Enum { eASPECT_21_9, eASPECT_4_3 };
  YEAGER_ENUM_TO_STRING(AspectRatio)
  YEAGER_STRING_TO_ENUM(AspectRatio)
  static float ToValue(Enum type);
};

/**
 *  @brief Represents a plane in the screen 
 */
struct OnScreenSpaceRange {
  UVector2 mSize;
  UVector2 mPosition;
};

/**
 * @brief Window general information is stored in this struct
 */
struct WindowInfo {
  Vector2 mEditorSize = Vector2(0.0f, 0.0f);
  Vector2 mLauncherSize = Vector2(0.0f, 0.0f);
  Vector2 mFrameBufferSize = Vector2(0.0f, 0.0f);
  Vector2 mFrameBufferPosition = Vector2(0.0f, 0.0f);
  IVector2 mWindowPosition = IVector2(0, 0);
  String mEditorTitle = YEAGER_EDITOR_DEFAULT_TITLE;
  String mLauncherTitle = YEAGER_LAUNCHER_DEFAULT_TITLE;
  GLFWcursorposfun mCursorFunc;
};

/**
 *  @brief Struct that holes glfw window hints to be parse around without problem
 */
struct WindowCreationHints {
  u_short mContextVersionMajor = 4;
  u_short mContextVersionMinor = 6;
  uint32_t mOpenGLProfile = GLFW_OPENGL_CORE_PROFILE;
  u_short mAntiAliasingSamples = 4;

#ifdef YEAGER_SYSTEM_MACOS
  bool mOpenGLForwandCompat = GL_TRUE;
#endif
};

class Window {
 public:
  Window(Yeager::ApplicationCore* application);
  ~Window();

  void Terminate();

  void GenerateWindow(String title, GLFWcursorposfun cursor, DFlags wndType);

  YEAGER_NODISCARD GLFWwindow* GetGLFWwindow() { return mWindowHandle; }

  Vector2 GetWindowSize() const;

  IVector2 GetWindowPos() const;

  void SetWindowPos(const IVector2& pos);

  bool GenerateWindow(const UVector2& size, const String& title);

  bool RegenerateMainWindow(const UVector2& size, const String& title) noexcept;

  void InitializeCallbacks(GLFWcursorposfun cursor);

  static void ResizeCallback(GLFWwindow* window, int width, int height);

  static void WindowMaximizeCallback(GLFWwindow* window, int maximized);

  static void WindowPosCallback(GLFWwindow* window, int xpos, int ypos);

  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

  static void HandleError(int code, Cchar description);

  bool CheckIfOpenGLContext();

  /**
   * @brief Regenerates the main window with the new given number of samples for the anti aliasing (ps: glfw needs to recreate the window to 
   * function), it returns true if the process was successed, or false if otherwise
   */
  bool ChangeAntiAliasingSamples(int samples);

  YEAGER_FORCE_INLINE bool ValidateAntiAliasingSampleValue(int samples) const;

  YEAGER_FORCE_INLINE WindowInfo* GetWindowInformationPtr() { return &sWindowInformation; }

  YEAGER_FORCE_INLINE WindowCreationHints* GetWindowCreationHintsPtr() { return &mWindowHints; }

  void BuildWindowHints();

 private:
  static WindowInfo sWindowInformation;
  static const std::vector<uint> sAcceptableSampleValues;

  WindowCreationHints mWindowHints;
  GLFWwindow* mWindowHandle = YEAGER_NULLPTR;
  Yeager::ApplicationCore* mApplication = YEAGER_NULLPTR;

  bool bMaximized = false;
  bool bFullScreen = false;
  bool bTerminated = false;
};
}  // namespace Yeager