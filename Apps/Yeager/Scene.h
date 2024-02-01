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

#include <algorithm>
#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Engine/Editor/ToolboxObj.h"
#include "Engine/Lighting/LightHandle.h"
#include "Engine/Media/AudioHandle.h"
#include "Engine/Renderer/Object.h"
#include "Serialization.h"

namespace Yeager {
class ApplicationCore;

enum SceneType { Scene2D, Scene3D, SceneError };
enum SceneRenderer { OpenGL3_3, OpenGL4, RendererError };
extern YgString SceneTypeToString(SceneType type);
extern YgString SceneRendererToString(SceneRenderer renderer);
extern SceneType StringToSceneType(YgString str);
extern SceneRenderer StringToSceneRenderer(YgString str);

struct SceneContext {
  YgString m_name = YEAGER_NULL_LITERAL;
  YgString m_file_path = YEAGER_NULL_LITERAL;
  YgString m_ProjectFolderPath = YEAGER_NULL_LITERAL;
  YgString m_ProjectRelativeConfigurationPath = YEAGER_NULL_LITERAL;
  YgString m_ProjectSavePath = YEAGER_NULL_LITERAL;
  YgString m_ProjectAuthor = YEAGER_NULL_LITERAL;
  SceneType m_ExplorerType = SceneType::Scene2D;
  SceneRenderer m_renderer = SceneRenderer::OpenGL3_3;
};

class Scene {
 public:
  Scene(YgString name, YgString Author, SceneType type, YgString folder_path, SceneRenderer renderer,
        Yeager::ApplicationCore* app);
  ~Scene();
  Scene() {}

  void Save();
  void Load(YgString path);
  void LoadEditorColorscheme(Interface* intr);

  SceneContext GetContext() { return m_Context; }
  void SetContextType(SceneType type);
  void SetContextRenderer(SceneRenderer renderer);
  void LoadSceneSave();
  YgString GetPathRelative(YgString path);
  YgString GetAssetsPath() { return m_AssetsFolderPath;}

  /**
   *  Scene Objects and Entities (Everything that is stored where is going to be saved) 
   */
  std::vector<std::shared_ptr<Yeager::Audio3DHandle>>* GetAudios3D();
  std::vector<std::shared_ptr<Yeager::AudioHandle>>* GetAudios();
  std::vector<std::shared_ptr<Yeager::Object>>* GetObjects();
  std::vector<std::shared_ptr<ToolBoxObject>>* GetToolboxs();
  std::vector<std::shared_ptr<Yeager::InstancedObject>>* GetInstancedObjects();
  std::vector<std::shared_ptr<Yeager::AnimatedObject>>* GetAnimatedObject();
  std::vector<std::shared_ptr<Yeager::InstancedAnimatedObject>>* GetInstancedAnimatedObjects();
  std::vector<std::shared_ptr<Yeager::PhysicalLightHandle>>* GetLightSources();

  std::vector<std::pair<ImporterThreaded*, Yeager::Object*>>* GetThreadImporters();
  std::vector<std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>>* GetThreadAnimatedImporters();
  void CheckThreadsAndTriggerActions();

  void CheckAndAwaitThreadsToFinish();

  void CheckScheduleDeletions();
  void CheckToolboxesScheduleDeletions();

  /* Check for duplicate elements in the vector and returns the positions of the duplicates */
  template <typename Type>
  std::vector<size_t> CheckDuplicatesEntities(std::vector<std::shared_ptr<Type>>* vec)
  {
    if (vec->empty() || vec->size() == 1)
      return std::vector<size_t>();
    /* Check if the template type have a operator== declared */
    if (!Yeager::CHECK::EqualExists<Type>::value) {
      Yeager::Log(WARNING, "Check for Duplicates Entities, class type does not have equal operator!");
      return std::vector<size_t>();
    }
    Yeager::Log(INFO, "Check for Duplicates Entities, class type have a equal operator!");
    std::vector<size_t> QueueToDeletion;

    /* Find duplicates, the right most element of the vector is the choosen one to delete */
    int index_cmp = 1;
    for (unsigned int x = 0; x < vec->size(); x++) {
      for (unsigned int y = index_cmp; y < vec->size(); y++) {
        Type* obj1 = vec->at(x).get();
        Type* obj2 = vec->at(y).get();
        if (obj1 == obj2)
          QueueToDeletion.push_back(y);
      }
    }

    return QueueToDeletion;
  }

  /* Attention using this function, it only removes the element for the vector, it does not handle the proper deletion of 
  objects and entities in the engine! */
  template <typename Type>
  void DeleteDuplicatesEntities(std::vector<std::shared_ptr<Type>>* vec)
  {
    std::vector<size_t> pos = CheckDuplicatesEntities(vec);
    for (const auto& index : pos) {
      vec->erase(vec->begin() + index);
      for (auto& index : pos) {
        index--;
      }
    }
  }

  void CheckDuplicatesLightSources();
  /* Will try to find sound files in the /Assets/Sound folder of the project and return a pair of the file name and the complete path */
  std::vector<std::pair<YgString, YgString>> VerifySoundsOptionsInAssetFolder();
  /* Will try to search for folders and model files inside of it on the /Assets/ImportedModels folder of the project, and return a pair of the file name and complete path */
  std::vector<std::pair<YgString, YgString>> VerifyImportedModelsOptionsInAssetsFolder();

 private:
  // The toolbox destroryed can be the selected one, if its stays selected after deletion, it will cause a read to nullptr!
  // This function disable the selected toolbox in the Explorer
  void CheckToolboxIsSelectedAndDisable(Yeager::ToolBoxObject* toolbox);

  void RemoveDuplicatesEntities();

  void ValidatesCommonFolders();
  YgString m_AssetsFolderPath = YEAGER_NULL_LITERAL;

  void VerifyAssetsSubFolders();

  std::vector<std::pair<ImporterThreaded*, Yeager::Object*>> m_ThreadImporters;
  std::vector<std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>> m_ThreadAnimatedImporters;

  SceneContext m_Context;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::vector<std::shared_ptr<Yeager::Audio3DHandle>> m_Audios3D;
  std::vector<std::shared_ptr<Yeager::AudioHandle>> m_Audios;
  std::vector<std::shared_ptr<Yeager::Object>> m_Objects;
  std::vector<std::shared_ptr<Yeager::InstancedObject>> m_InstancedObjects;
  std::vector<std::shared_ptr<Yeager::AnimatedObject>> m_AnimatedObject;
  std::vector<std::shared_ptr<Yeager::InstancedAnimatedObject>> m_InstancedAnimatedObjects;
  std::vector<std::shared_ptr<Yeager::ToolBoxObject>> m_Toolboxes;
  std::vector<std::shared_ptr<Yeager::PhysicalLightHandle>> m_LightSources;

  YgString GetConfigurationFilePath(YgString path);
};
}  // namespace Yeager