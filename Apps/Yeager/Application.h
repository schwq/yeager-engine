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
#include "Engine/Editor/Camera.h"
#include "Engine/Editor/Explorer.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Physics/PhysXHandle.h"
#include "Engine/Renderer/Window.h"
#include "InputHandle.h"
#include "Launcher.h"
#include "Scene.h"

namespace Yeager {

/// @brief Represent the current state for the Application, if its running, paused, crashed ect
struct YgApplicationState {
  enum Enum { eAPPLICATION_RUNNING, eAPPLICATION_STOPPED, eAPPLICATION_CRASHED };
};
/// @brief Represent the current mode of the Application, used most for control the current window to appear to the user
struct YgApplicationMode {
  enum Enum {
    eAPPLICATION_EDITOR,
    eAPPLICATION_LAUNCHER,
    eAPPLICATION_SETTINGS,
    eAPPLICATION_GAME,
    eAPPLICATION_LOADING
  };
};

struct WorldCharacterMatrices {
  YgMatrix4 Projection = YEAGER_IDENTITY_MATRIX4x4;
  YgMatrix4 View = YEAGER_IDENTITY_MATRIX4x4;
  YgVector3 ViewerPos = YEAGER_ZERO_VECTOR3;
};

struct LoadedProjectHandle {
  YgString m_ProjectName = YEAGER_NULL_LITERAL;
  YgString m_ProjectFolderPath = YEAGER_NULL_LITERAL;
  YgString m_ProjectConfigurationPath = YEAGER_NULL_LITERAL;
};

/* ApplicationCore pointers should never have a default argument! This leads to seg faults around the code when the developer (like me) 
 forgets to link the application pointer to the class because the IDE havent alert about a missing argument! */
class ApplicationCore {
 public:
  ApplicationCore();
  ~ApplicationCore();
  ApplicationCore(ApplicationCore&&) = delete;

  void Setup();
  /**
   * Engine information are stored in $HOME/.YeagerEngine in linux plataforms, and in %appdata%/.YeagerEngine in windows plataforms
  */
  void CreateDirectoriesAndFiles();
  bool ShouldRender();
  /* Ever scene must have some default entities, like a proper skybox and some light source*/
  void LoadSceneDefaultEntities();

  LauncherProjectPicker RequestLauncher();
  void Render();
  void Terminate() {}

  Interface* GetInterface();
  Input* GetInput();
  Window* GetWindow();
  EditorExplorer* GetExplorer();
  EditorCamera* GetCamera();
  Scene* GetScene();

  YgApplicationMode::Enum GetMode() noexcept;
  YgApplicationState::Enum GetState() noexcept;

  void SetMode(YgApplicationMode::Enum mode) noexcept;
  void SetState(YgApplicationState::Enum state) noexcept;
  void CheckGLAD();

  /** Returns external folder location {Path to engine external folder location}/{Engine folder name}/External.
  On Unix systems the engine external folder location is the $HOME variable, and in windows system, %appdata% location 
  The engine folder name in Unix system is .YeagerEngine to hide it, in windows system, the folder is just YeagerEngine, without the point */
  YgString GetPathRelativeToExternalFolder(YgString path) const;

  YEAGER_FORCE_INLINE YgMatrix4 GetProjection() const { return m_WorldMatrices.Projection; }
  YEAGER_FORCE_INLINE YgMatrix4 GetView() const { return m_WorldMatrices.View; }
  YEAGER_FORCE_INLINE YgString GetExternalFolder() const { return m_EngineExternalFolder; }
  YEAGER_FORCE_INLINE std::vector<LoadedProjectHandle>* GetLoadedProjectsHandles() { return &m_LoadedProjectsHandles; }
  YEAGER_FORCE_INLINE Serialization* GetSerial() { return m_Serial; }
  YEAGER_FORCE_INLINE PhysXHandle* GetPhysXHandle() { return m_PhysXHandle; }
  YEAGER_FORCE_INLINE AudioEngineHandle* GetAudioEngine() { return m_AudioEngine; }
  YEAGER_FORCE_INLINE std::vector<std::pair<std::shared_ptr<Shader>, YgString>>* GetConfigShaders()
  {
    return &m_ConfigShaders;
  }
  YEAGER_FORCE_INLINE float GetTimeBeforeLoop() const { return m_TimeBeforeRender; }
  YEAGER_FORCE_INLINE long long GetFrameCurrentCount() const { return m_FrameCurrentCount; }

  void AddConfigShader(std::shared_ptr<Yeager::Shader> shader, YgString var) noexcept;
  Shader* ShaderFromVarName(YgString var);

  physx::PxController* m_Controller = YEAGER_NULLPTR;

 private:
  YgString GetExternalFolderLocation();
  YgString m_EngineExternalFolder = YEAGER_NULL_LITERAL;
  YgString RequestWindowEngineName(const LauncherProjectPicker& project);

  void ValidatesExternalEngineFolder();
  void ManifestShaderProps(Yeager::Shader* shader);
  void OpenGLFunc();
  void OpenGLClear();
  void BuildApplicationCoreCompoments();
  void UpdateDeltaTime();
  void UpdateWorldMatrices();
  void UpdateListenerPosition();
  void DrawObjects();
  void BuildAndDrawLightSources();
  void VerifyCollisions();
  void ManifestAllShaders();

  float m_DeltaTime = 0.0f;
  float m_LastFrame = 0.0f;
  float m_TimeBeforeRender = 0.0f;
  long long m_FrameCurrentCount = 0;

  /**   Every shader have a var name associated with it, so it can been called on the ShaderFromVar(var name) */
  std::vector<std::pair<std::shared_ptr<Shader>, YgString>> m_ConfigShaders;
  std::vector<LoadedProjectHandle> m_LoadedProjectsHandles;

  WorldCharacterMatrices m_WorldMatrices;
  Serialization* m_Serial = YEAGER_NULLPTR;
  Interface* m_Interface = YEAGER_NULLPTR;
  Input* m_Input = YEAGER_NULLPTR;
  Window* m_Window = YEAGER_NULLPTR;
  EditorExplorer* m_EditorExplorer = YEAGER_NULLPTR;
  EditorCamera* m_EditorCamera = YEAGER_NULLPTR;
  AudioEngineHandle* m_AudioEngine = YEAGER_NULLPTR;
  Scene* m_Scene = YEAGER_NULLPTR;
  Launcher* m_Launcher = YEAGER_NULLPTR;
  PhysXHandle* m_PhysXHandle = YEAGER_NULLPTR;

  YgApplicationState::Enum m_state = YgApplicationState::eAPPLICATION_RUNNING;
  YgApplicationMode::Enum m_mode = YgApplicationMode::eAPPLICATION_LAUNCHER;
};
}  // namespace Yeager