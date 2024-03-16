//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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

#define YEAGER_EXPLORER_MAX_STRING_INPUT 150

extern String GetExplorerSymbolForToolbox(EntityObjectType::Enum type);

class EditorExplorer {
 public:
  EditorExplorer(Yeager::ApplicationCore* app);
  EditorExplorer() = default;
  ~EditorExplorer();
  void DrawExplorer();

  constexpr YEAGER_FORCE_INLINE ToolboxHandle* GetSelectedToolbox() const { return m_ToolboxSelected; }
  YEAGER_FORCE_INLINE void ResetSelectedToolbox() { m_ToolboxSelected = YEAGER_NULLPTR; }

 private:
  void AddGeometryObjectWindow();
  void HandleObjectCreation();
  void HandleGeometryCreation();
  void HandleGeometryCleanup();
  void AddImportedObjectWindow();
  void AddAudioWindow();
  void HandleAudioCreation();
  void HandleAudioCleanup();
  void HandleSelectableOptions(const String& name);
  void DrawToolboxesList();
  void CleanupAfterObjectCreation();
  const String CreateToolboxLabel(Yeager::ToolboxHandle* obj);

  void AsynchronousAwaitUserFolder(FileExtensionType type);
  void StartFolderSelection(const String& title);

  void BuildInstancedObjectTransformation(std::vector<Transformation*>& pos);
  void DeleteInstancedObjectTransformation(std::vector<Transformation*>* pos);

  Yeager::ToolboxHandle* m_ToolboxSelected = YEAGER_NULLPTR;
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
  String m_NewObjectName = YEAGER_NULL_LITERAL;
  String m_NewObjectPath = YEAGER_NULL_LITERAL;
  String m_NewObjectTexturePath = YEAGER_NULL_LITERAL;

  static bool m_AwaitingUserChoiceFile;
  pfd::open_file* m_Selection = YEAGER_NULLPTR;

  std::vector<std::pair<String, String>> m_SelectableOptions;
};
}  // namespace Yeager