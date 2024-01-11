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
#include "Engine/Renderer/Importer.h"

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

<<<<<<< Updated upstream
  SceneContext GetContext() { return m_context; }
  Serialization GetSerial() { return m_serial; }
=======
  SceneContext GetContext() { return m_Context; }
  void SetContextType(SceneType type);
  void SetContextRenderer(SceneRenderer renderer);
  void LoadSceneSave();
  YgString GetPathRelative(YgString path);

  void VerifyAssetsSubFolders();
  /* Will try to find sound files in the /Assets/Sound folder of the project and return a pair of the file name and the complete path */
  std::vector<std::pair<YgString, YgString>> VerifySoundsOptionsInAssetFolder();
  /* Will try to search for folders and model files inside of it on the /Assets/ImportedModels folder of the project, and return a pair of the file name and complete path */
  std::vector<std::pair<YgString, YgString>> VerifyImportedModelsOptionsInAssetsFolder();

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
  std::vector<std::shared_ptr<Yeager::LightSource>>* GetLightSources();

  std::vector<std::pair<ImporterThreaded*, Yeager::Object*>>* GetThreadImporters();
  std::vector<std::pair<ImporterThreadedAnimated*, Yeager::AnimatedObject*>>* GetThreadAnimatedImporters();
  void CheckThreadsAndTriggerActions();
>>>>>>> Stashed changes

 private:
  SceneContext m_context;
  Serialization m_serial;

<<<<<<< Updated upstream
  inline yg_string GetSceneFilePath();
=======
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
  std::vector<std::shared_ptr<Yeager::LightSource>> m_LightSources;
  YgString GetConfigurationFilePath(YgString path);
>>>>>>> Stashed changes
};
}  // namespace Yeager