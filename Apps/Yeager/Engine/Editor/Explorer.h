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

class EditorExplorer {
 public:
  EditorExplorer(Application* app);
  void DrawExplorer();

  Yeager::ToolBoxObject* toolbox_selected;

 private:
  void AddGeometryObjectWindow();
  void AddImportedObjectWindow();
  void AddAudioWindow();
  void WarningExplorer(yg_string msg);
  bool m_first_time_toolbox = true;
  bool m_add_imported_object_window_open = false;
  bool m_imported_object_flip_tex = true;
  bool m_everything_fine_to_create = true;
  bool m_operation_canceled = false;
  bool m_add_audio_window_open = false;
  bool m_add_geometry_window_open = false;
  bool m_add_geometry_shape_cube = false;
  bool m_looped_audio = false;
  bool m_add_audio_is_3d = false;
  yg_string m_new_object_name;
  yg_string m_new_object_path;
  yg_string m_new_object_texture_path;
  Application* m_app;
};