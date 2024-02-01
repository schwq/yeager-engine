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

#include "../../Common/Common.h"
#include "../../Engine/Interface/Interface.h"
#include "../../Engine/Renderer/Object.h"
#include "../Media/AudioHandle.h"
#include "ToolboxObj.h"

namespace Yeager {
class ApplicationCore;
class Interface;

class EditorExplorer {
 public:
  EditorExplorer(Yeager::ApplicationCore* app);
  EditorExplorer() {}
  void DrawExplorer();

  constexpr inline ToolBoxObject* GetSelectedToolbox() const { return m_ToolboxSelected; }
  void ResetSelectedToolbox() { m_ToolboxSelected = YEAGER_NULLPTR;  }

 private:
  void AddGeometryObjectWindow();
  void AddImportedObjectWindow();
  void AddAudioWindow();
  void WarningExplorer(YgString msg);

  Yeager::ToolBoxObject* m_ToolboxSelected = YEAGER_NULLPTR;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  bool m_FirstTimeToolbox = true;
  bool m_AddImportedObjectWindowOpen = false;
  bool m_ImportedObjectFlipTexture = true;
  bool m_EverythingFineToCreate = true;
  bool m_OperationCanceled = false;
  bool m_AddAudioWindowOpen = false;
  bool m_AddGeometryWindowOpen = false;
  bool m_AddGeometryShapeCube = false;
  bool m_LoopedAudio = false;
  bool m_AddAudioIs3D = false;
  bool m_AddObjectIsAnimated = false;
  bool m_AddObjectIsInstanced = false;
  int m_InstancedGridFactor = 1;
  int m_InstancedObjectsCount = 1;
  YgString m_NewObjectName = YEAGER_NULL_LITERAL;
  YgString m_NewObjectPath = YEAGER_NULL_LITERAL;
  YgString m_NewObjectTexturePath = YEAGER_NULL_LITERAL;

  std::vector<std::pair<YgString, YgString>> m_SelectableOptions;
};
}  // namespace Yeager