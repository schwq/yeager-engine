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

#pragma once

#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Serialization.h"

class Yeager::Serialization;

namespace Yeager {
enum SceneType { Scene2D, Scene3D };
enum SceneRenderer { OpenGL3_3, OpenGL4 };

struct SceneContext {
  yg_string m_name;
  yg_string m_file_path;
  SceneType m_type;
  SceneRenderer m_renderer;
};

class Scene {
 public:
  Scene(yg_string name, SceneType type, SceneRenderer renderer);
  ~Scene();

  void Save();
  void Load(yg_string path);
  void LoadEditorColorscheme(Interface* interface);

  SceneContext GetContext() { return m_context; }
  Serialization GetSerial() { return m_serial; }

 private:
  SceneContext m_context;
  Serialization m_serial;

  inline yg_string GetSceneFilePath();
};
}  // namespace Yeager