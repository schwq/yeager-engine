#include "common/common.h"

#include "application.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/window.h"
#include <GLFW/glfw3.h>

static void glfwErrorCallback(int error, const char* description)
{
  VLOG_F(ERROR, description);
}

int main(int argc, char* argv[])
{
  static Application* app = new Application();
  Input* input = new Input(app);
  Window* window = new Window(kWindowX, kWindowY, glfwErrorCallback,
                              app->GetInput()->MouseCallback);

  RendererEngine* engine = new RendererEngine(RendererEngineName::kOpenGL, app);
  app->GetRendererEngine()->checkGLAD();
  Interface* interfaceUI = new Interface(window, app);
  EditorCamera* camera = new EditorCamera(app);

  app->SetupApplication(input, window, engine, camera, interfaceUI);

  app->GetRendererEngine()->Render();
  VLOG_F(INFO, "Shutdown program");

  delete app;
  delete input;
  delete window;
  delete engine;
  delete interfaceUI;
  delete camera;
  return EXIT_SUCCESS;
}