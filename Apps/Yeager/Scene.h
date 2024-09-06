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

#include <algorithm>
#include "Common/Common.h"
#include "Common/LogEngine.h"
#include "Common/Utilities.h"
#include "Engine/Editor/Camera.h"
#include "Engine/Editor/NodeHierarchy.h"
#include "Engine/Editor/ToolboxObj.h"
#include "Engine/Lighting/LightHandle.h"
#include "Engine/Media/AudioHandle.h"
#include "Engine/Renderer/Object.h"
#include "Serialization.h"

namespace Yeager {
class ApplicationCore;
class Skybox;

template <typename Type>
using VecSharedPtr = std::vector<std::shared_ptr<Type>>;
template <typename Ty1, typename Ty2>
using VecPair = std::vector<std::pair<Ty1, Ty2>>;

enum SceneType { Scene2D, Scene3D, SceneError };
enum SceneRenderer { OpenGL3_3, OpenGL4, RendererError };
extern String SceneTypeToString(SceneType type);
extern String SceneRendererToString(SceneRenderer renderer);
extern SceneType StringToSceneType(String str);
extern SceneRenderer StringToSceneRenderer(String str);

struct SceneContext {
  String Name = YEAGER_NULL_LITERAL;
  String FilePath = YEAGER_NULL_LITERAL;
  String ProjectFolderPath = YEAGER_NULL_LITERAL;
  String ProjectRelativeConfigurationPath = YEAGER_NULL_LITERAL;
  String ProjectSavePath = YEAGER_NULL_LITERAL;
  String ProjectAuthor = YEAGER_NULL_LITERAL;
  SceneType ProjectSceneType = SceneType::Scene2D;
  SceneRenderer ProjectSceneRenderer = SceneRenderer::OpenGL3_3;
  TimePointType TimeOfCreation;
};

static SceneContext InitializeContext(String name, String author, SceneType type, String folderPath,
                                      SceneRenderer renderer, TimePointType dateOfCreation);

class Scene {
 public:
  Scene(Yeager::ApplicationCore* app);

  ~Scene();
  Scene() {}

  void BuildScene(const LauncherProjectPicker& project);

  void Save();
  void Load(String path);
  void LoadEditorColorscheme(Interface* intr);
  void Terminate();

  SceneContext* GetContext() { return &m_Context; }
  void SetContextType(SceneType type);
  void SetContextRenderer(SceneRenderer renderer);
  void LoadSceneSave();
  String GetAssetsPath() { return m_AssetsFolderPath; }

  /**
   *  Scene Objects and Entities (Everything that is stored where is going to be saved) 
   */
  VecSharedPtr<Yeager::Audio3DHandle>* GetAudios3D();
  VecSharedPtr<Yeager::AudioHandle>* GetAudios();
  VecSharedPtr<Yeager::Object>* GetObjects();
  VecSharedPtr<ToolboxHandle>* GetToolboxs();
  VecSharedPtr<Yeager::AnimatedObject>* GetAnimatedObject();
  VecSharedPtr<Yeager::PhysicalLightHandle>* GetLightSources();

  VecPair<ImporterThreaded*, Yeager::Object*>* GetThreadImporters();
  VecPair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>* GetThreadAnimatedImporters();
  void CheckThreadsAndTriggerActions();

  void CheckAndAwaitThreadsToFinish();

  void CheckScheduleDeletions();
  void CheckToolboxesScheduleDeletions();

  /* Check for duplicate elements in the vector and returns the positions of the duplicates */
  template <typename Type>
  std::optional<std::vector<size_t>> CheckDuplicatesEntities(VecSharedPtr<Type>* vec)
  {
    if (vec->empty() || vec->size() == 1)
      return std::nullopt;
    /* Check if the template type have a operator== declared */
    if (!Yeager::CHECK::EqualExists<Type>::value) {
      Yeager::LogDebug(WARNING, "Check for Duplicates Entities, class type does not have equal operator!");
      return std::nullopt;
    }
    Yeager::LogDebug(INFO, "Check for Duplicates Entities, class type have a equal operator!");
    std::vector<size_t> QueueToDeletion;

    /* Find duplicates, the right most element of the vector is the choosen one to delete */
    int index_cmp = 1;
    for (Uint x = 0; x < vec->size(); x++) {
      for (Uint y = index_cmp; y < vec->size(); y++) {
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
  void DeleteDuplicatesEntities(VecSharedPtr<Type>* vec)
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
  VecPair<String, String> VerifySoundsOptionsInAssetFolder();

  /* Will try to search for folders and model files inside of it on the /Assets/ImportedModels folder of the project, and return a pair of the file name and complete path */
  VecPair<String, String> VerifyImportedModelsOptionsInAssetsFolder();

  std::shared_ptr<PlayerCamera> GetPlayerCamera() { return m_PlayerCamera; }

  std::vector<std::shared_ptr<NodeComponent>>* GetNodeHierarchy() { return &m_NodeHierarchy; }

  YEAGER_FORCE_INLINE constexpr bool SceneContainsRootNode() const { return m_SceneContainsRootNode; }
  std::shared_ptr<NodeComponent> GetRootNode() { return m_RootNodeOfScene; }

  /* I dont think about a scenario where is going to be possible to just dont have a root node
  but will leave it this way, for now. */
  void SetSceneContainsRootNode(bool contains, std::shared_ptr<NodeComponent> root)
  {
    m_SceneContainsRootNode = contains;
    m_RootNodeOfScene = root;
  }

  String GetTextureCacheFolderPath() const;

  void BuildSceneFromTemplate(const TemplateHandle& handle);

  void DrawSkybox(Yeager::Shader* shader, const Matrix3& view, const Matrix4& projection);
  std::shared_ptr<Yeager::Skybox> GetSkybox() { return m_Skybox; }

  TemplateHandle GetTemplateHandle() const { return m_Template;  }

  void SetSkybox(std::shared_ptr<Yeager::Skybox> skybox) { m_Skybox = skybox; }

 private:
  TemplateHandle m_Template;

  // The toolbox destroryed can be the selected one, if its stays selected after deletion, it will cause a read to nullptr!
  // This function disable the selected toolbox in the Explorer
  void CheckToolboxIsSelectedAndDisable(Yeager::ToolboxHandle* toolbox);
  void ValidatesCommonFolders();
  void RemoveDuplicatesEntities();
  void VerifyAssetsSubFolders();
  void VerifyCacheSubFolders();
  void InitializeRootNode();


  std::shared_ptr<Yeager::Skybox> m_Skybox; // Every scene must have a skybox!
  String GetConfigurationFilePath(String path) const;

  SceneContext m_Context;
  bool m_SceneContainsRootNode = false;
  bool m_SceneWasTerminated = false;
  String m_AssetsFolderPath = YEAGER_NULL_LITERAL;
  std::shared_ptr<PlayerCamera> m_PlayerCamera = YEAGER_NULLPTR;
  std::shared_ptr<NodeComponent> m_RootNodeOfScene = YEAGER_NULLPTR;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;

  VecPair<ImporterThreaded*, Yeager::Object*> m_ThreadImporters;
  VecPair<ImporterThreadedAnimated*, Yeager::AnimatedObject*> m_ThreadAnimatedImporters;

  VecSharedPtr<Yeager::Audio3DHandle> m_Audios3D;
  VecSharedPtr<Yeager::AudioHandle> m_Audios;
  VecSharedPtr<Yeager::Object> m_Objects;
  VecSharedPtr<Yeager::AnimatedObject> m_AnimatedObject;
  VecSharedPtr<Yeager::ToolboxHandle> m_Toolboxes;
  VecSharedPtr<Yeager::PhysicalLightHandle> m_LightSources;

  std::vector<std::shared_ptr<NodeComponent>> m_NodeHierarchy;
};
}  // namespace Yeager