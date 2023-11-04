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

// clang-format on 
int main(int argc, char* argv[])
{
  Yeager::ApplicationCoreSetup setup;
  auto app = std::make_shared<Yeager::ApplicationCore>();

  auto input = std::make_shared<Input>(app.get());
  auto window = std::make_shared<Window>(ygWindowWidth, ygWindowHeight, app->GetInput()->MouseCallback);
  auto scene = std::make_shared<Yeager::Scene>("DefaultScene", Yeager::Scene3D, Yeager::OpenGL3_3, app.get());
  auto renderer = std::make_shared<RendererEngine>(RendererEngineName::kOpenGL, app.get());
  app->GetRenderer()->checkGLAD();
  auto interface = std::make_shared<Interface>(window.get(), app.get());
  auto camera = std::make_shared<EditorCamera>(app.get());
  auto explorer = std::make_shared<EditorExplorer>(app.get());


  setup.m_input = input;
  setup.m_window = window;
  setup.m_renderer = renderer;
  setup.m_scene = scene;
  app->GetRenderer()->checkGLAD();
  setup.m_interface = interface;
  setup.m_camera = camera;
  setup.m_explorer = explorer;
  app->Setup(setup);
  app->GetRenderer()->Render();
  
  return EXIT_SUCCESS;
}
