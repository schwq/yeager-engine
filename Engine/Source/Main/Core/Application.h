//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//
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

#include "Common/Algorithm/BruteForceSearch.h"

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include "Components/Kernel/Caching/Cache.h"
#include "Components/Kernel/Memory/Allocator.h"
#include "Components/Kernel/Network/NetworkSocket.h"
#include "Components/Kernel/Process/WpThread.h"
#include "Components/Physics/PhysXHandle.h"
#include "Components/Player/PlayableObject.h"
#include "Components/Text/TextRendering.h"
#include "Debug/GL/DebbugingGL.h"
#include "Editor/Camera/Camera.h"
#include "Editor/UI/Explorer.h"
#include "Editor/UI/Interface.h"
#include "Editor/UI/Locale.h"
#include "Main/IO/InputHandle.h"
#include "Main/Scene/Scene.h"
#include "Main/Settings/DefaultValues.h"
#include "Main/Settings/Request.h"
#include "Main/Settings/Settings.h"
#include "Main/Window/Launcher.h"
#include "Main/Window/Window.h"
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
  Matrix4 mProjection = YEAGER_IDENTITY_MATRIX4x4;
  Matrix4 mView = YEAGER_IDENTITY_MATRIX4x4;
  Vector3 mViewerPos = YEAGER_ZERO_VECTOR3;
};

/** @brief Handles the yeager engine projects loaded in the machine, represents the name, folder, configuration path */
struct LoadedProjectHandle {
  String mProjectName = YEAGER_NULL_LITERAL;
  String mProjectFolderPath = YEAGER_NULL_LITERAL;
  String mProjectConfigurationPath = YEAGER_NULL_LITERAL;
};

struct ForcedEntryProject {
  bool bActivated = false;
  String mProjectName = YEAGER_NULL_LITERAL;
};

struct DebugTitleString {
  bool bActivated = false;
  String mTitle = YEAGER_NULL_LITERAL;
};

struct DebugVirtualMachine {
  bool bActivated = false;
  float mTimer = 10.0f;
};

struct ProgramArguments {
  ForcedEntryProject mForcedEntry;
  std::optional<Yeager::LauncherProjectPicker> SearchForcedEntryProject(Yeager::ApplicationCore* application);
  DebugTitleString mDebugTitle;
  DebugVirtualMachine mVirtualMachine;
  NetworkStartupConnection mStartupNetwork;
};

struct UsableVariables {
  String mImGuiConfigurationPath = YEAGER_NULL_LITERAL;
  String mLoadeProjectsConfigurationPath = YEAGER_NULL_LITERAL;
  String mEngineVariablesConfigurationPath = YEAGER_NULL_LITERAL;
};

/**
    @brief Panic is a critical error that the engine cannot run anymore withit, so we forced a crash!
*/
void PanicCrashReport(const std::exception& exc);

/** 
    @brief The main class of the engine, handles everything! Every class is called in this class functions.
    @attention ApplicationCore pointers should never have a default argument! This leads to seg faults around the code when the developer
    forgets to link the application pointer to the class because the IDE havent alert about a missing argument! 
*/
class ApplicationCore {
 public:
  ApplicationCore(int argc, char* argv[]);
  ~ApplicationCore();
  ApplicationCore(ApplicationCore&&) = delete;

  void ProcessArguments(int argc, char* argv[]);
  Uint IterateArguments(const String& arg, Uint index);
  std::optional<const String> AcessableArgumentArray(Uint index);

  void Setup();
  bool ShouldRender();

  void ShowCommonTextOnScreen();

  std::optional<LauncherProjectPicker> RequestLauncher();
  void UpdateTheEngine();

  void SetMode(ApplicationMode::Enum mode) noexcept;
  void SetState(ApplicationState::Enum state) noexcept;
  void CheckGLADIntegrity();

  /** Some getters functions so the other classes have access to the application */

  ApplicationMode::Enum GetMode() const noexcept;
  ApplicationState::Enum GetState() const noexcept;

  EditorExplorer* GetExplorer();
  Interface* GetInterface();
  Input* GetInput();
  Window* GetWindow();
  BaseCamera* GetCamera();
  Scene* GetScene();
  Serialization* GetSerial();
  Settings* GetSettings();
  RequestHandle* GetRequestHandle();
  DefaultValues* GetDefaults();
  PhysXHandle* GetPhysXHandle();
  AudioEngineHandle* GetAudioEngineHandle();
  physx::PxController* GetController();
  AudioEngine* GetAudioFromEngine();

  Locale GetLocale() { return mCurrentLocale; }

  float GetTimeBeforeLoop() const;
  long long GetFrameCurrentCount() const;

  void DetachPlayerCamera();
  void AttachPlayerCamera(std::shared_ptr<PlayerCamera> camera);
  void AddConfigShader(std::shared_ptr<Yeager::Shader> shader, const String& var) noexcept;

  void BeginEngineTimer();
  float GetSecondsElapsedSinceStart() const;

  /**
    @brief World matrices represent the projection, view and the position of the viewer 
    TODO: this must be in the player class, so multiple views can be created, for example for multiplayer
  */
  YEAGER_NODISCARD WorldCharacterMatrices GetWorldMatrices() const { return mWorldMatrices; }

  using ConfigShaders = std::vector<
      std::pair<std::shared_ptr<Shader>, String>>;  // Big type name, better way of making functions declaration smaller

  /** 
    @brief Returns the configuration shaders been used by the application,  its a vector of pairs with shared_ptr<Shader> and a string represeting the name
  */
  YEAGER_FORCE_INLINE ConfigShaders* GetConfigShaders() { return &mConfigShaders; }

  /**
    @brief Shaders are loaded into the engine trough a configuration file, each one have a variable name associated with it. By giving the right variable name,
    this function returns a pointer to the shader associated
  */
  YEAGER_NODISCARD Shader* ShaderFromVarName(String var);

  /**
    @brief During startup, the engine looks into the loaded projects configuration file to find about the projects created in the machine current running
  */
  YEAGER_NODISCARD std::vector<LoadedProjectHandle>* GetLoadedProjectsHandles();

  /**
    @brief The arguments are evaluated during the startup of this class, the ProgramArguments struct holds the values set to each type of argument option
    For example, the option of forced entry in a project (quick way to bypass the launcher screen) holds if this options is turn on and which project to forced entry 
  */
  YEAGER_NODISCARD ProgramArguments* GetArguments() { &mArguments; }

  /**
    @brief Engine variables are mostly defined paths for configuration or serialization  to be used
  */
  YEAGER_NODISCARD UsableVariables GetVariables() const { return mEngineVariables; }

  Uint GetReturnCode() { return mApplicationReturnCode; }

 private:
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

  void ProcessArgumentsDuringRender();

 private:
  UsableVariables mEngineVariables;
  ProgramArguments mArguments;
  TextRenderer mCommonTextOnScreen;

  using TimeElapsed = std::chrono::time_point<std::chrono::system_clock>;
  TimeElapsed mTimeElapsedSinceStart;

  bool mTimerHasStarted = false;
  float mDeltaTime = 0.0f;
  float mLastFrame = 0.0f;
  float mTimeBeforeRender = 0.0f;
  long long mFrameCurrentCount = 0;

  /**   
    @brief Every shader have a var name associated with it, so it can been called on the ShaderFromVar(var name) 
  */
  ConfigShaders mConfigShaders;
  std::vector<LoadedProjectHandle> mLoadedProjectsHandles;

  std::vector<String> mAllArguments;

  YEAGER_USING_SHARED_PTR

  SharedPtr<physx::PxController> mController = YEAGER_NULLPTR;
  SharedPtr<Serialization> mSerial = YEAGER_NULLPTR;
  SharedPtr<Interface> mInterface = YEAGER_NULLPTR;
  SharedPtr<Input> mInput = YEAGER_NULLPTR;
  SharedPtr<Window> mWindow = YEAGER_NULLPTR;
  SharedPtr<EditorExplorer> mEditorExplorer = YEAGER_NULLPTR;
  SharedPtr<BaseCamera> mBaseCamera = YEAGER_NULLPTR;
  SharedPtr<EditorCamera> mEditorCamera = YEAGER_NULLPTR;
  SharedPtr<RequestHandle> mRequest = YEAGER_NULLPTR;
  SharedPtr<DefaultValues> mDefaults = YEAGER_NULLPTR;
  SharedPtr<AudioEngineHandle> mAudioEngine = YEAGER_NULLPTR;
  SharedPtr<Scene> mScene = YEAGER_NULLPTR;
  SharedPtr<Launcher> mLauncher = YEAGER_NULLPTR;
  SharedPtr<PhysXHandle> mPhysXHandle = YEAGER_NULLPTR;
  SharedPtr<Settings> mSettings = YEAGER_NULLPTR;
  SharedPtr<AudioEngine> mAudiosFromEngine = YEAGER_NULLPTR;

  WorldCharacterMatrices mWorldMatrices;
  ApplicationState::Enum mCurrentState = ApplicationState::eAPPLICATION_RUNNING;
  ApplicationMode::Enum mCurrentMode = ApplicationMode::eAPPLICATION_LAUNCHER;

  Locale mCurrentLocale;

  Uint mApplicationReturnCode = 0;
};
}  // namespace Yeager