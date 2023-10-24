//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
//    Copyright (C) 2023
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

// clang-format off
#include "Common/Common.h"
#include "Application.h"
#include "Engine/Renderer/Render.h"
#include "Engine/Renderer/Window.h"

// clang-format on 
static void glfwErrorCallback(int error, yg_cchar description) {
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
  EditorExplorer* explorer = new EditorExplorer(app);
  ApplicationSetup setup;
  setup.ptr_camera = camera;
  setup.ptr_input = input;
  setup.ptr_interface = interfaceUI;
  setup.ptr_explorer = explorer;
  setup.ptr_window = window;
  setup.ptr_engine = engine;
  app->SetupApplication(setup);
  app->GetRendererEngine()->Render();
  VLOG_F(INFO, "Shutdown program");
  delete input;
  delete engine;
  delete camera;
  delete window;
  delete interfaceUI;
  delete app;
  return EXIT_SUCCESS;
}
