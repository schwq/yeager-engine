#pragma once

#include "common/common.h"
#include "engine/renderer/window.h"
#include "engine/renderer/renderer.h"
#include "input.h"
#include "interface.h"
#include "engine/editor/camera.h"

class RendererEngine;
class Window;
class Input;
class Interface;
class EditorCamera;

enum class ApplicationCurrentMode {
    kEditorMode,
    kErrorMode,
    kAwaitMode,
    kSettingsMode,
    kLauncherMode,
    kCeaseMode
};

class Application {
  public:
    Application();
    ~Application();
    bool ShouldRendererActive();
    Window *GetWindowManager() { return m_window; }
    RendererEngine *GetRendererEngine() { return m_engine; }
    Input *GetInput() { return m_input; }
    Interface *GetInterface() { return m_interface; }
    ApplicationCurrentMode GetCurrentMode() { return m_current_mode; }
    const void SetCurrentMode(ApplicationCurrentMode mode);
    EditorCamera *GetEditorCamera() { return m_camera; }

  private:
    ApplicationCurrentMode m_current_mode = ApplicationCurrentMode::kAwaitMode;
    EditorCamera *m_camera;
    Window *m_window;
    RendererEngine *m_engine;
    Input *m_input;
    Interface *m_interface;
};
