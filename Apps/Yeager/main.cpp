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

#include "Application.h"
#include "Common/Common.h"
#include "Engine/Renderer/Render.h"
#include "Engine/Renderer/Window.h"

int main(int argc, char* argv[])
{
  loguru::init(argc, argv);
  auto app = std::make_shared<Application>();
  auto input = std::make_shared<Input>(app.get());

  auto window = std::make_shared<Window>(kWindowX, kWindowY, app->GetInput()->MouseCallback);

  auto engine = std::make_shared<RendererEngine>(RendererEngineName::kOpenGL, app.get());
  app->GetRendererEngine()->checkGLAD();
  auto interfaceUI = std::make_shared<Interface>(window.get(), app.get());
  auto camera = std::make_shared<EditorCamera>(app.get());
  auto explorer = std::make_shared<EditorExplorer>(app.get());
  ApplicationSetup setup;
  setup.ptr_camera = camera.get();
  setup.ptr_input = input.get();
  setup.ptr_interface = interfaceUI.get();
  setup.ptr_explorer = explorer.get();
  setup.ptr_window = window.get();
  setup.ptr_engine = engine.get();
  app->SetupApplication(setup);
  app->GetRendererEngine()->Render();
  VLOG_F(INFO, "Shutdown program");
  return EXIT_SUCCESS;
}
