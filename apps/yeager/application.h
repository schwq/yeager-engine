#pragma once
// clang-format off
#include "common/common.h"
#include "common/utils.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/shapes.h"
#include "engine/editor/camera.h"
#include "engine/editor/editor_console.h"
#include "engine/editor/editor_explorer.h"
#include "engine/lighting/lighting.h"
#include "engine/lighting/lighting_source.h"
#include "engine/renderer/imported_object.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/window.h"
#include "input.h"
#include "interface.h"

class YeagerShape;
class RendererEngine;
class Window;
class Input;
class Interface;
class EditorCamera;
class EditorExplorer;
class EditorConsole;
class ImportedObject;
struct ConsoleMessageType;
class LightingManager;
class PointLightSource;

// clang-format on

struct ApplicationSetup {
  Input* ptr_input;
  Window* ptr_window;
  RendererEngine* ptr_engine;
  Interface* ptr_interface;
  EditorCamera* ptr_camera;
  EditorExplorer* ptr_explorer;
  EditorConsole* ptr_console;
  LightingManager* ptr_lighting;
};

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

  constexpr Window* GetWindowManager() const { return m_window; }
  constexpr RendererEngine* GetRendererEngine() const { return m_engine; }
  constexpr Input* GetInput() const { return m_input; }
  constexpr Interface* GetInterface() const { return m_interface; }
  constexpr ApplicationCurrentMode GetCurrentMode() const {
    return m_current_mode;
  }
  constexpr EditorCamera* GetEditorCamera() const { return m_camera; }
  void SetupApplication(ApplicationSetup setup);
  bool ShouldRendererActive();
  bool EnterKeyPressed();

  constexpr EditorConsole* GetConsole() const { return m_console; }
  constexpr EditorExplorer* GetExplorer() const { return m_explorer; }
  constexpr LightingManager* GetLighting() const { return m_lighting; }
  constexpr std::vector<YeagerShape>* GetVectorUserCube() {
    return &m_cubes_user_created;
  }
  constexpr std::vector<YeagerTexture2D>* GetVectorUserTexture2D() {
    return &m_textures_2d_user_created;
  }
  constexpr std::vector<ImportedObject>* GetImportedObjects() {
    return &m_imported_objects;
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
  LightingManager* m_lighting = nullptr;

  std::vector<YeagerShape> m_cubes_user_created;
  std::vector<YeagerTexture2D> m_textures_2d_user_created;
  std::vector<ImportedObject> m_imported_objects;
};
