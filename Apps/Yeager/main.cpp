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
#include "Scene.h"

// clang-format on 
int main(int argc, char* argv[])
{
 
  Yeager::ApplicationCoreSetup setup;
  auto app = std::make_shared<Yeager::ApplicationCore>();
  auto input = std::make_shared<Input>(app.get());
  std::shared_ptr<Window> window = std::make_shared<Window>(ygWindowWidth, ygWindowHeight, "Yeager Engine",app->GetInput()->MouseCallback);
  std::shared_ptr<Yeager::Scene> scene = std::make_shared<Yeager::Scene>("DefaultScene", Yeager::Scene2D, Yeager::OpenGL3_3, app.get());
  app->CheckGLAD();
  auto intr = std::make_shared<Interface>(window.get(), app.get());
  auto camera = std::make_shared<EditorCamera>(app.get());
  auto explorer = std::make_shared<EditorExplorer>(app.get());

  setup.m_input = input;
  setup.m_window = window;
  setup.m_scene = scene;
  setup.m_interface = intr;
  setup.m_camera = camera;
  setup.m_explorer = explorer;
  setup.m_scene = scene;
  app->Setup(setup);
  app->Render();
  
  return EXIT_SUCCESS;
}
