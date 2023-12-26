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
extern SceneType StringToScreneType(YgString str);
extern SceneRenderer StringToSceneRenderer(YgString str);

struct SceneContext {
  YgString m_name = YEAGER_NULL_LITERAL;
  YgString m_file_path = YEAGER_NULL_LITERAL;
  SceneType m_type = SceneType::Scene2D;
  SceneRenderer m_renderer = SceneRenderer::OpenGL3_3;
};

class Scene {
 public:
  Scene(YgString name, SceneType type, SceneRenderer renderer, Yeager::ApplicationCore* app);
  ~Scene();
  Scene() {}
  Yeager::Scene& operator=(const Yeager::Scene& rhs)
  {
    this->m_Context.m_name = rhs.m_Context.m_name;
    this->m_Context.m_type = rhs.m_Context.m_type;
    this->m_Context.m_renderer = rhs.m_Context.m_renderer;
    this->m_Context.m_file_path = rhs.m_Context.m_file_path;
    this->m_Serial = Yeager::Serialization(rhs.m_Application);
  }

  void Save();
  void Load(YgString path);
  void LoadEditorColorscheme(Interface* intr);

  SceneContext GetContext() { return m_Context; }
  Serialization GetSerial() { return m_Serial; }
  void SetContextType(SceneType type);
  void SetContextRenderer(SceneRenderer renderer);

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

 private:
  SceneContext m_Context;
  Serialization m_Serial;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::vector<std::shared_ptr<Yeager::Audio3DHandle>> m_Audios3D;
  std::vector<std::shared_ptr<Yeager::AudioHandle>> m_Audios;
  std::vector<std::shared_ptr<Yeager::Object>> m_Objects;
  std::vector<std::shared_ptr<Yeager::InstancedObject>> m_InstancedObjects;
  std::vector<std::shared_ptr<Yeager::AnimatedObject>> m_AnimatedObject;
  std::vector<std::shared_ptr<Yeager::InstancedAnimatedObject>> m_InstancedAnimatedObjects;
  std::vector<std::shared_ptr<Yeager::ToolBoxObject>> m_Toolboxes;
  std::vector<std::shared_ptr<Yeager::LightSource>> m_LightSources;

  inline YgString GetSceneFilePath();
};
}  // namespace Yeager