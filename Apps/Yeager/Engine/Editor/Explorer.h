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

#include "../../Application.h"
#include "../../Common/Common.h"
#include "../Media/AudioHandle.h"
#include "../Renderer/ImportedObj.h"
#include "ToolboxObj.h"

class Application;
class Interface;
class ImportedObject;

struct ExplorerObject {
  ExplorerObject(Yeager::ToolBoxObject toolbox_object);
  static yg_uint m_object_count;
  yg_uint m_id;
  bool m_selected = false;
  Yeager::ToolBoxObject m_object_toolbox;
};

class EditorExplorer {
 public:
  EditorExplorer(Application* app);
  void AddObject(ExplorerObject object);
  void AddObject(Yeager::ToolBoxObject toolbox_object);
  void DrawExplorer();
  ExplorerObject* toolbox_selected;

 private:
  void AddCubeWindow();
  void AddImportedObjectWindow();
  void AddLightPointSource();
  void AddAudioWindow();
  void WarningExplorer(yg_string msg);

  std::vector<ExplorerObject> m_objects;
  bool m_first_time_toolbox = true;
  bool m_add_cube_window_open = false;
  bool m_add_imported_object_window_open = false;
  bool m_everything_fine_to_create = true;
  bool m_add_light_point_source_window_open = false;
  bool m_operation_canceled = false;
  bool m_add_audio_window_open = false;
  bool m_looped_audio = false;
  yg_string m_new_object_name;
  yg_string m_new_object_path;
  yg_string m_new_object_texture_path;
  Application* m_app;
};