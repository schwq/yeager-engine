#pragma once

#include "application.h"
#include "common/common.h"
#include "engine/renderer/window.h"

enum class InterfaceMode { kEditorMode, kLauncherMode, kSettingsMode, kAwaitMode, kErrorMode };

class Interface {
  public:
    Interface(Window *window, Application *app);
    ~Interface(){};

    bool getInitStatus() { return m_initialize; }
    void RenderUI();
    InterfaceMode GetCurrentMode() { return m_current_mode; }
    const void SetCurrentMode(InterfaceMode mode) { m_current_mode = mode; }

  private:
    ImGuiIO m_imgui_io;
    bool m_initialize = false;
    InterfaceMode m_current_mode = InterfaceMode::kAwaitMode;
    Application *m_app;

    void RenderAwait();
    void RenderLauncher(){};
    void RenderEditor();
    void RenderSettings(){};
    void RenderError(){};
};