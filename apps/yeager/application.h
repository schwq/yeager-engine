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

  Window *GetWindowManager() const { return m_window; }
  RendererEngine *GetRendererEngine() const { return m_engine; }
  Input *GetInput() const { return m_input; }
  Interface *GetInterface() const { return m_interface; }
  ApplicationCurrentMode GetCurrentMode() const { return m_current_mode; }
  EditorCamera *GetEditorCamera() const { return m_camera; }
  void SetupApplication(Input *input, Window *window, RendererEngine *engine,
                        EditorCamera *camera, Interface *interfaceUI);
  const void SetCurrentMode(ApplicationCurrentMode mode);
  bool ShouldRendererActive();

 private:
  ApplicationCurrentMode m_current_mode = ApplicationCurrentMode::kEditorMode;
  EditorCamera *m_camera = nullptr;
  Window *m_window = nullptr;
  RendererEngine *m_engine = nullptr;
  Input *m_input = nullptr;
  Interface *m_interface = nullptr;
};
