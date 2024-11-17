//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
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

static const String GetExplorerSymbolForToolbox(EntityObjectType::Enum type);
static const String CreateToolboxLabel(Yeager::ToolboxHandle* obj);

class EditorExplorer {
 public:
  EditorExplorer(Yeager::ApplicationCore* app);
  EditorExplorer() = default;
  ~EditorExplorer();
  void DrawExplorer();

  YEAGER_CONSTEXPR YEAGER_FORCE_INLINE ToolboxHandle* GetSelectedToolbox() const { return m_ToolboxSelected; }
  YEAGER_FORCE_INLINE void ResetSelectedToolbox() { m_ToolboxSelected = YEAGER_NULLPTR; }

  void QuickLoadObject(const std::filesystem::path& path);

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

  void AsynchronousAwaitUserFolder(FileExtensionType type);
  void StartFolderSelection(const String& title);
  void StartFolderSelectionFromTexture(const String& title);
  void AsynchronousAwaitUserTextureFolder();

  YEAGER_USING_SHARED_PTR
  void BuildInstancedObjectTransformation(std::vector<SharedPtr<Transformation3D>>& pos);
  void DeleteInstancedObjectTransformation(std::vector<SharedPtr<Transformation3D>>& pos);

  void CreateObject();
  void CreateAnimatedObject();
  void CreatePlayableObject();

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
  bool m_AddObjectIsPlayable = false;
  bool m_AddObjectIsInstanced = false;
  int m_InstancedGridFactor = 1;
  int m_InstancedObjectsCount = 1;
  String m_NewObjectName = YEAGER_NULL_LITERAL;
  String m_NewObjectPath = YEAGER_NULL_LITERAL;
  String m_NewObjectTexturePath = YEAGER_NULL_LITERAL;
  ObjectCreationConfiguration m_CreationConfiguration;

  bool m_AwaitingUserChoiceFile = false;
  bool m_AwaintingUserChoiceFolder = false;

  SharedPtr<pfd::open_file> m_FileSelection = YEAGER_NULLPTR;
  SharedPtr<pfd::select_folder> m_FolderSelection = YEAGER_NULLPTR;
  std::vector<std::pair<String, String>> m_SelectableOptions;
};
}  // namespace Yeager