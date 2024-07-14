//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//
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
#include "DefaultValues.h"
#include "Engine/Editor/Camera.h"
#include "Engine/Editor/Explorer.h"
#include "Engine/Interface/Interface.h"
#include "Engine/Physics/PhysXHandle.h"
#include "Engine/Renderer/Player/PlayableObject.h"
#include "Engine/Renderer/Text/TextRendering.h"
#include "Engine/Renderer/Window.h"
#include "InputHandle.h"
#include "Kernel/Caching/Cache.h"
#include "Launcher.h"
#include "Request.h"
#include "Scene.h"
#include "Settings.h"

namespace Yeager {

/** @brief Represent the current state for the Application, if its running, paused, crashed ect */
struct ApplicationState {
  enum Enum { eAPPLICATION_RUNNING, eAPPLICATION_STOPPED, eAPPLICATION_CRASHED, eAPPLICATION_UNDEFINED };
  YEAGER_ENUM_TO_STRING(ApplicationState)
  YEAGER_STRING_TO_ENUM(ApplicationState)
};

/** @brief Represent the current mode of the Application, used most for control the current window to appear to the user */
struct ApplicationMode {
  enum Enum {
    eAPPLICATION_EDITOR,
    eAPPLICATION_LAUNCHER,
    eAPPLICATION_SETTINGS,
    eAPPLICATION_GAME,
    eAPPLICATION_LOADING,
    eAPPLICATION_UNDEFINED
  };
  YEAGER_ENUM_TO_STRING(ApplicationMode)
  YEAGER_STRING_TO_ENUM(ApplicationMode)
};

/** @brief Represent the matrices that the character needs to view the world */
struct WorldCharacterMatrices {
  Matrix4 Projection = YEAGER_IDENTITY_MATRIX4x4;
  Matrix4 View = YEAGER_IDENTITY_MATRIX4x4;
  Vector3 ViewerPos = YEAGER_ZERO_VECTOR3;
};

/** @brief Handles the yeager engine projects loaded in the machine, represents the name, folder, configuration path */
struct LoadedProjectHandle {
  String ProjectName = YEAGER_NULL_LITERAL;
  String ProjectFolderPath = YEAGER_NULL_LITERAL;
  String ProjectConfigurationPath = YEAGER_NULL_LITERAL;
};

struct ForcedEntryProject {
  bool Activated = false;
  String ProjectName = YEAGER_NULL_LITERAL;
};

struct DebugTitleString {
  bool Activated = false;
  String Title = YEAGER_NULL_LITERAL;
};

/** @brief The main class of the engine, handles everything! Every class is called in this class functions.
 * @attention ApplicationCore pointers should never have a default argument! This leads to seg faults around the code when the developer
 forgets to link the application pointer to the class because the IDE havent alert about a missing argument! */
class ApplicationCore {
 public:
  ApplicationCore(int argc, char* argv[]);
  ~ApplicationCore();
  ApplicationCore(ApplicationCore&&) = delete;

  void ProcessArguments(int argc, char* argv[]);
  Uint IterateArguments(const String& arg, Uint index);
  std::optional<const String> AcessableArgumentArray(Uint index);

  void Setup();
  /** @brief Engine information are stored in $HOME/.YeagerEngine in linux plataforms, and in %appdata%/.YeagerEngine in windows plataforms */
  void CreateDirectoriesAndFiles();
  bool ShouldRender();

  void ShowCommonTextOnScreen();

  std::optional<LauncherProjectPicker> RequestLauncher();
  void UpdateTheEngine();

  ApplicationMode::Enum GetMode() const noexcept;
  ApplicationState::Enum GetState() const noexcept;

  void SetMode(ApplicationMode::Enum mode) noexcept;
  void SetState(ApplicationState::Enum state) noexcept;
  void CheckGLADIntegrity();

  /** @brief Returns external folder location {Path to engine external folder location}/{Engine folder name}/External.
  On Unix systems the engine external folder location is the $HOME variable, and in windows system, %appdata% location 
  The engine folder name in Unix system is .YeagerEngine to hide it, in windows system, the folder is just YeagerEngine, without the point */
  String GetPathRelativeToExternalFolder(String path) const;
  EditorExplorer* GetExplorer();

  /** Some getters functions so the other classes have access to the application */

  Interface* GetInterface();
  Input* GetInput();
  Window* GetWindow();
  BaseCamera* GetCamera();
  Scene* GetScene();
  String GetSettingsExternalFolder() const;
  Matrix4 GetProjection() const;
  Matrix4 GetView() const;
  String GetExternalFolder() const;
  std::vector<LoadedProjectHandle>* GetLoadedProjectsHandles();
  Serialization* GetSerial();
  Settings* GetSettings();
  RequestHandle* GetRequestHandle();
  DefaultValues* GetDefaults();
  PhysXHandle* GetPhysXHandle();
  AudioEngineHandle* GetAudioEngineHandle();
  physx::PxController* GetController();
  AudioEngine* GetAudioFromEngine();
  float GetTimeBeforeLoop() const;
  long long GetFrameCurrentCount() const;

  using ConfigShaders = std::vector<std::pair<std::shared_ptr<Shader>, String>>;
  /** @brief Returns the configuration shaders been used by the application, 
   * its a vector of pairs with shared_ptr<Shader> and a string represeting the name*/
  YEAGER_FORCE_INLINE ConfigShaders* GetConfigShaders() { return &m_ConfigShaders; }

  Shader* ShaderFromVarName(String var);
  void DetachPlayerCamera();
  void AttachPlayerCamera(std::shared_ptr<PlayerCamera> camera);
  void AddConfigShader(std::shared_ptr<Yeager::Shader> shader, const String& var) noexcept;
  void BeginEngineTimer();
  float GetSecondsElapsedSinceStart() const;

  ForcedEntryProject* GetForcedEntryProject() { return &m_ForcedEntry; }
  std::optional<Yeager::LauncherProjectPicker> SearchForcedEntryProject();
  DebugTitleString* GetDebugTitleString() { return &m_DebugTitleString; }

 private:
  String GetExternalFolderLocation();
  String RequestWindowEngineName(const LauncherProjectPicker& project);
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
  void ManifestAllShaders();
  void TerminatePosRender();
  void SetupCamera();
  void UpdateCamera();
  void PrepareSceneToLoad(const LauncherProjectPicker& project);

 private:
  ForcedEntryProject m_ForcedEntry;
  DebugTitleString m_DebugTitleString;

  TextRenderer m_CommonTextOnScreen;

  String m_EngineExternalFolder = YEAGER_NULL_LITERAL;
  String m_EngineConfigurationPath = YEAGER_NULL_LITERAL;

  using TimeElapsed = std::chrono::time_point<std::chrono::system_clock>;
  TimeElapsed m_TimeElapsedSinceStart;

  bool m_TimerHasStarted = false;
  float m_DeltaTime = 0.0f;
  float m_LastFrame = 0.0f;
  float m_TimeBeforeRender = 0.0f;
  long long m_FrameCurrentCount = 0;

  /**   Every shader have a var name associated with it, so it can been called on the ShaderFromVar(var name) */
  ConfigShaders m_ConfigShaders;
  std::vector<LoadedProjectHandle> m_LoadedProjectsHandles;

  std::vector<String> m_AllArguments;

  template <class U>
  using SharedPtr = std::shared_ptr<U>;

  SharedPtr<physx::PxController> m_Controller = YEAGER_NULLPTR;
  SharedPtr<Serialization> m_Serial = YEAGER_NULLPTR;
  SharedPtr<Interface> m_Interface = YEAGER_NULLPTR;
  SharedPtr<Input> m_Input = YEAGER_NULLPTR;
  SharedPtr<Window> m_Window = YEAGER_NULLPTR;
  SharedPtr<EditorExplorer> m_EditorExplorer = YEAGER_NULLPTR;
  SharedPtr<BaseCamera> m_BaseCamera = YEAGER_NULLPTR;
  SharedPtr<EditorCamera> m_EditorCamera = YEAGER_NULLPTR;
  SharedPtr<RequestHandle> m_Request = YEAGER_NULLPTR;
  SharedPtr<DefaultValues> m_Defaults = YEAGER_NULLPTR;
  SharedPtr<AudioEngineHandle> m_AudioEngine = YEAGER_NULLPTR;
  SharedPtr<Scene> m_Scene = YEAGER_NULLPTR;
  SharedPtr<Launcher> m_Launcher = YEAGER_NULLPTR;
  SharedPtr<PhysXHandle> m_PhysXHandle = YEAGER_NULLPTR;
  SharedPtr<Settings> m_Settings = YEAGER_NULLPTR;
  SharedPtr<AudioEngine> m_AudiosFromEngine = YEAGER_NULLPTR;

  WorldCharacterMatrices m_WorldMatrices;
  ApplicationState::Enum m_state = ApplicationState::eAPPLICATION_RUNNING;
  ApplicationMode::Enum m_mode = ApplicationMode::eAPPLICATION_LAUNCHER;
};
}  // namespace Yeager