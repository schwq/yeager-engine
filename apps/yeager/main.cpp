// clang-format off
#include "common/common.h"
#include "application.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/window.h"
// clang-format on 
static void glfwErrorCallback(int error, const char* description) {
  VLOG_F(ERROR, description);
}

int main(int argc, char* argv[]) {
  loguru::init(argc, argv);
  static Application* app = new Application();
  Input* input = new Input(app);
  Window* window = new Window(kWindowX, kWindowY, glfwErrorCallback,
                              app->GetInput()->MouseCallback);

  RendererEngine* engine = new RendererEngine(RendererEngineName::kOpenGL, app);
  app->GetRendererEngine()->checkGLAD();
  Interface* interfaceUI = new Interface(window, app);
  EditorCamera* camera = new EditorCamera(app);
  EditorConsole* console = new EditorConsole();
  EditorExplorer* explorer = new EditorExplorer(app);
  LightingManager* lighting = new LightingManager();

  ApplicationSetup setup;
  setup.ptr_camera = camera;
  setup.ptr_input = input;
  setup.ptr_console = console;
  setup.ptr_interface = interfaceUI;
  setup.ptr_explorer = explorer;
  setup.ptr_window = window;
  setup.ptr_engine = engine;
  setup.ptr_lighting = lighting;

  app->SetupApplication(setup);
  app->GetRendererEngine()->Render();
  VLOG_F(INFO, "Shutdown program");

  delete app;
  delete input;
  delete window;
  delete engine;
  delete interfaceUI;
  delete camera;
  delete lighting;
  return EXIT_SUCCESS;
}