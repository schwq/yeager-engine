#pragma once

#include "common/common.h"
#include "application.h"
#include "engine/renderer/window.h"

class Application;

enum class InputCurrentMode {
  kEditorMode,
  kCeaseMode,
  kLauncherMode,
  kSettingsMode,
  kErrorMode,
  kAwaitMode
};

class Input {
 public:
  Input(Application *app);
  ~Input(){};

  void ProcessInputRender(Window *window, float delta);
  static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
  InputCurrentMode GetCurrentMode() { return m_current_mode; }
  const void SetCurrentMode(InputCurrentMode mode) { m_current_mode = mode; };
  const void SetCursorCanDisappear(bool should);

 private:
  InputCurrentMode m_current_mode = InputCurrentMode::kAwaitMode;
  static Application *m_app;
  static float lastY;
  static float lastX;
  static bool firstMouse;
  static uint m_framesCount;
  bool m_cursor_can_disappear = true;
};