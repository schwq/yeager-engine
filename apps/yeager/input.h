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
    Input(Application *app) : m_app(app){};
    ~Input(){};

    void ProcessInputRender(Window *window, float delta);
    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
    InputCurrentMode GetCurrentMode() { return m_current_mode; }
    const void SetCurrentMode(InputCurrentMode mode) { m_current_mode = mode; };

  private:
    InputCurrentMode m_current_mode = InputCurrentMode::kAwaitMode;
    Application *m_app;
    static float lastY;
    static float lastX;
    static bool firstMouse;
};