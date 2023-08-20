#pragma once

#include "application.h"
#include "common/common.h"
#include "engine/renderer/window.h"
#include "engine/renderer/texture.h"
#include "engine/editor/editor_explorer.h"

struct ExplorerObject;

enum class InterfaceMode {
  kEditorMode,
  kLauncherMode,
  kSettingsMode,
  kAwaitMode,
  kErrorMode
};

struct InterfaceButton {
  InterfaceButton(String name, String text) : m_text(text), m_name(name){};
  bool CenteredButton();
  bool AddButton();
  float TextWidth();
  String m_text;
  String m_name;
};

struct InterfaceImage {
  InterfaceImage(const char *path);
  void LoadInterfaceImage();
  void LoadInterfaceCenteredImage();
  int m_image_width = 0;
  int m_image_height = 0;
  GLuint m_image_texture = 0;
};

class Interface {
 public:
  Interface(Window *window, Application *app);
  ~Interface();

  bool getInitStatus() { return m_initialize; }
  void RenderUI();
  InterfaceMode GetCurrentMode() { return m_current_mode; }
  const void SetCurrentMode(InterfaceMode mode) { m_current_mode = mode; }

 private:
  bool m_initialize = false;
  float size_pixels = 13.0f;
  static uint m_frames;
  InterfaceMode m_current_mode = InterfaceMode::kLauncherMode;
  Application *m_app;
  bool m_dont_move_windows_editor = false;
  bool m_comment_window_open = false;
  String comment;

  void RenderAwait();
  void RenderLauncher();
  void RenderEditor();
  void RenderSettings(){};
  void RenderError(){};

  void logButton(InterfaceButton button);
  void StyleImGui();
  void CenteredText(String text);
  void AlignForWidth(float width, float alignment = 0.5f);
  void CreateSpaceX(uint count);
  void RenderDebugger();
};
