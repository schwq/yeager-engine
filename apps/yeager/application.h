#pragma once

#include "common/common.h"
#include "common/utils.h"
#include "engine/editor/camera.h"
#include "engine/editor/editor_console.h"
#include "engine/editor/editor_explorer.h"
#include "engine/renderer/imported_object.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/shapes.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/window.h"
#include "input.h"
#include "interface.h"

class RendererEngine;
class Window;
class Input;
class Interface;
class EditorCamera;
class EditorExplorer;
class YeagerCube;
class EditorConsole;
class ImportedObject;

struct ApplicationSetup {
  Input* ptr_input;
  Window* ptr_window;
  RendererEngine* ptr_engine;
  Interface* ptr_interface;
  EditorCamera* ptr_camera;
  EditorExplorer* ptr_explorer;
  EditorConsole* ptr_console;
};

enum class ConsoleLogSender { kUserLog, kSystemLog, kNoneLog };

constexpr String ConsoleLogSenderToString(ConsoleLogSender sender);

enum class ApplicationCurrentMode {
  kEditorMode = 0,
  kErrorMode = 1,
  kAwaitMode = 2,
  kSettingsMode = 3,
  kLauncherMode = 4,
  kCeaseMode = 5
};

class Application {
 public:
  Application();
  ~Application();

  const void SetCurrentMode(ApplicationCurrentMode mode);

  Window* GetWindowManager() const { return m_window; }
  RendererEngine* GetRendererEngine() const { return m_engine; }
  Input* GetInput() const { return m_input; }
  Interface* GetInterface() const { return m_interface; }
  ApplicationCurrentMode GetCurrentMode() const { return m_current_mode; }
  EditorCamera* GetEditorCamera() const { return m_camera; }
  void SetupApplication(ApplicationSetup setup);
  bool ShouldRendererActive();
  bool EnterKeyPressed();

  EditorConsole* GetConsole() const { return m_console; }
  EditorExplorer* GetExplorer() const { return m_explorer; }
  std::vector<YeagerCube*>* GetVectorUserCube() {
    return &m_cubes_user_created;
  }
  std::vector<EngineTexture2D*>* GetVectorUserTexture2D() {
    return &m_textures_2d_user_created;
  }
  std::vector<ImportedObject*>* GetImportedObjects() {
    return &m_imported_objects;
  }

  template <typename... T>
  void YeagerLog(int verbosity, ConsoleLogSender sender,
                 fmt::format_string<T...> fmt, T&&... args) {
    auto str = fmt::format(fmt, std::forward<T>(args)...);
    String log(str);
    m_console->SetLogString("[" + ConsoleLogSenderToString(sender) + "] " +
                            log);

    if (!(sender == ConsoleLogSender::kUserLog)) {
      VLOG_F(verbosity, log.c_str());
    }
  }

 private:
  ApplicationCurrentMode m_current_mode = ApplicationCurrentMode::kEditorMode;
  EditorCamera* m_camera = nullptr;
  Window* m_window = nullptr;
  RendererEngine* m_engine = nullptr;
  Input* m_input = nullptr;
  Interface* m_interface = nullptr;
  EditorConsole* m_console = nullptr;
  EditorExplorer* m_explorer = nullptr;

  std::vector<YeagerCube*> m_cubes_user_created;
  std::vector<EngineTexture2D*> m_textures_2d_user_created;
  std::vector<ImportedObject*> m_imported_objects;
};
