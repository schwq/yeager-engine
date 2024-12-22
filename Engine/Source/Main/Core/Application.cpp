#include "Application.h"
#include "Components/Lighting/LightHandle.h"
#include "Components/Renderer/AnimationEngine/AnimationEngine.h"
#include "Components/Renderer/Objects/Object.h"
#include "Components/Renderer/Skybox/Skybox.h"
#include "Components/TerrainGen/ProceduralTerrain.h"
#include "Components/TerrainGen/TerrainGenThread.h"

using namespace Yeager;

String ApplicationState::ToString(ApplicationState::Enum type)
{
  switch (type) {
    case eAPPLICATION_RUNNING:
      return "Running";
    case eAPPLICATION_STOPPED:
      return "Stopped";
    case eAPPLICATION_CRASHED:
      return "Crashed";
    case eAPPLICATION_UNDEFINED:
    default:
      return "Undefined";
  }
}

ApplicationState::Enum ApplicationState::ToEnum(const String& str)
{
  switch (StringToInterger(str.c_str())) {
    case StringToInterger("Running"):
      return eAPPLICATION_RUNNING;
    case StringToInterger("Stopped"):
      return eAPPLICATION_STOPPED;
    case StringToInterger("Crashed"):
      return eAPPLICATION_CRASHED;
    case StringToInterger("Undefined"):
    case StringToInterger(YEAGER_NULL_LITERAL):
    default:
      return eAPPLICATION_UNDEFINED;
  }
}

String ApplicationMode::ToString(ApplicationMode::Enum type)
{
  switch (type) {
    case eAPPLICATION_EDITOR:
      return "Editor";
    case eAPPLICATION_GAME:
      return "Game";
    case eAPPLICATION_LAUNCHER:
      return "Launcher";
    case eAPPLICATION_LOADING:
      return "Loading";
    case eAPPLICATION_SETTINGS:
      return "Settings";
    case eAPPLICATION_UNDEFINED:
    default:
      return "Undefined";
  }
}

ApplicationMode::Enum ApplicationMode::ToEnum(const String& str)
{
  switch (StringToInterger(str.c_str())) {
    case StringToInterger("Editor"):
      return eAPPLICATION_EDITOR;
    case StringToInterger("Game"):
      return eAPPLICATION_GAME;
    case StringToInterger("Launcher"):
      return eAPPLICATION_LAUNCHER;
    case StringToInterger("Loading"):
      return eAPPLICATION_LOADING;
    case StringToInterger("Settings"):
      return eAPPLICATION_SETTINGS;
    case StringToInterger("Undefined"):
    case StringToInterger(YEAGER_NULL_LITERAL):
    default:
      return eAPPLICATION_UNDEFINED;
  }
}

std::optional<Yeager::LauncherProjectPicker> ProgramArguments::SearchForcedEntryProject(
    Yeager::ApplicationCore* application)
{
  std::vector<OpenProjectsDisplay> projects = Yeager::ReadProjectsToDisplay(
      GetPathFromLocal("/Configuration/Projects/LoadedProjectsPath.yml").value(), application);

  for (const auto& proj : projects) {
    if (proj.Name == mForcedEntry.mProjectName) {
      LauncherProjectPicker project(proj);
      return project;
    }
  }
  return std::nullopt;
}

ApplicationCore::ApplicationCore(int argc, char* argv[])
    : mSerial(BaseAllocator::MakeSharedPtr<Serialization>(this)),
      mScene(BaseAllocator::MakeSharedPtr<Scene>(this)),
      mWindow(BaseAllocator::MakeSharedPtr<Window>(this)),
      mCommonTextOnScreen(this),
      mCurrentLocale(this, ELanguangeRegion::EN_US, true)
{
  InitializeRandomGenerator();
  ProcessArguments(argc, argv);
  ValidatesExternalEngineFolder();
  mSerial->ReadLoadedProjectsHandles(GetPathFromLocal("/Configuration/Projects").value());
  Setup();
}

std::optional<const String> ApplicationCore::AcessableArgumentArray(Uint index)
{
  if (index > mAllArguments.size())
    return std::nullopt;
  return mAllArguments.at(index);
}

void ApplicationCore::ProcessArguments(int argc, char* argv[])
{

  if (argc > 1) {
    mAllArguments.assign(argv + 1, argv + argc);

    Uint index = 0;
    while (index < mAllArguments.size()) {
      std::optional<const String> arg = AcessableArgumentArray(index);
      if (!arg.has_value()) {
        Yeager::Log(ERROR, "Cannot process arguments properly, trying to access array from undefined index!");
        return;
      }
      index += IterateArguments(arg.value(), index);
    }
  }
}

Uint ApplicationCore::IterateArguments(const String& arg, Uint index)
{
  switch (StringToInterger(arg.c_str())) {
    case StringToInterger("-ForcedEntry"): {
      std::optional<const String> entry = AcessableArgumentArray(index + 1);
      if (!entry.has_value()) {
        Yeager::Log(ERROR, "Cannot process arguments properly, trying to access array from undefined index!");
        return 1;
      }
      mArguments.mForcedEntry.bActivated = true;
      mArguments.mForcedEntry.mProjectName = entry.value();
      return 2;
    }
    case StringToInterger("-DebugTitleString"): {

      std::optional<const String> title = AcessableArgumentArray(index + 1);
      if (!title.has_value()) {
        Yeager::Log(ERROR, "Cannot process arguments properly, trying to access array from undefined index!");
        return 1;
      }

      mArguments.mDebugTitle.bActivated = true;
      mArguments.mDebugTitle.mTitle = title.value();
      return 2;
    }
    case StringToInterger("-DebugVirtualMachine"): {
      std::optional<const String> vm = AcessableArgumentArray(index + 1);
      if (vm.has_value() && vm.value() == "valgrind") {
        Yeager::Log(INFO, "Current debugging virtual machine is set to VALGRIND!");
        mArguments.mVirtualMachine.bActivated = true;
        return 2;
      }
      Yeager::Log(ERROR, "Cannot process arguments properly, trying to access array from undefined index!");
      return 1;
    }
    case StringToInterger("-FromSource"): {
      g_FromSourceCode = true;
      Yeager::LogDebug(INFO, "The engine is being called from the source code!");
      return 1;
    }

    case StringToInterger("-NetworkSocket"): {
      std::optional<const String> role = AcessableArgumentArray(index + 1);
      std::optional<const String> ip = AcessableArgumentArray(index + 2);
      std::optional<const String> port = AcessableArgumentArray(index + 3);
      if (role.has_value() && ip.has_value() && port.has_value()) {
        mArguments.mStartupNetwork.mIpAddr = ip.value();
        mArguments.mStartupNetwork.mPort = std::stoi(port.value());
        mArguments.mStartupNetwork.mRole = NetworkCommunicationRole::CLIENT;
        mArguments.mStartupNetwork.Start();
        mArguments.mStartupNetwork.mSocket.Send("Hello from Yeager Engine!");
        std::vector<unsigned int> v = {1, 2, 3, 4, 5};
        mArguments.mStartupNetwork.mSocket.Send(v);
        return 4;
      }
      return 1;
    }

    default:
      Yeager::Log(WARNING, "Invalid argument {}, ignoring it", arg);
  }

  return 1;
}

void ApplicationCore::ValidatesExternalEngineFolder() {}

std::optional<LauncherProjectPicker> ApplicationCore::RequestLauncher()
{
  String title = "Yeager Launcher";
  if (mArguments.mDebugTitle.bActivated) {
    title += " " + mArguments.mDebugTitle.mTitle;
  }

  const ImVec2 launcherWindowSize = mSettings->GetEngineConfiguration()->LauncherWindowSize;
  mWindow->GetWindowInformationPtr()->mLauncherSize = Vector2(launcherWindowSize.x, launcherWindowSize.y);
  mLauncher = BaseAllocator::MakeSharedPtr<Yeager::Launcher>(launcherWindowSize.x, launcherWindowSize.y, title, this);

  if (mArguments.mForcedEntry.bActivated) {
    std::optional<Yeager::LauncherProjectPicker> project = mArguments.SearchForcedEntryProject(this);
    if (project.has_value()) {
      mInterface->SetSceneFileText(Yeager::FileContentToString(project.value().m_ProjectConfigurationPath));
      return project.value();
    }
    Yeager::Log(ERROR, "Cannot find project in configuration file: {}", mArguments.mForcedEntry.mProjectName);
  }

  mLauncher->Render();

  if (ShouldRender()) {
    // The user have not decided to exit the program during the launcher phase
    return mLauncher->GetSelectedProject();
  } else {
    // The user have requested a exit program, it returns a "fake" project with the (UserWantToExit) variable set to true
    mLauncher->GetSelectedProjectPtr()->UserWantToExit = true;
    return std::nullopt;
  }
}

String ApplicationCore::RequestWindowEngineName(const LauncherProjectPicker& project)
{
  String scene_renderer_str = Yeager::SceneRendererToString(project.m_SceneRenderer);
  std::replace(scene_renderer_str.begin(), scene_renderer_str.end(), '_', ' ');
  String engine_new_name = "Yeager Engine : " + project.m_Name + " / " + scene_renderer_str;
  if (mArguments.mDebugTitle.bActivated)
    engine_new_name += " " + mArguments.mDebugTitle.mTitle;
  return engine_new_name;
}

void ApplicationCore::BuildApplicationCoreCompoments()
{
  mSettings = BaseAllocator::MakeSharedPtr<Settings>(this);
  mRequest = BaseAllocator::MakeSharedPtr<RequestHandle>(this);
  mInput = BaseAllocator::MakeSharedPtr<Input>(this);

  mSerial->ReadEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());

  mWindow->GenerateWindow("Yeager Engine", mInput->MouseCallback, YEAGER_GENERATE_LAUNCHER_WINDOW);
  mInput->InitializeCallbacks();
  mAudioEngine = BaseAllocator::MakeSharedPtr<AudioEngineHandle>();
  mAudioEngine->InitAudioEngine();
  mAudiosFromEngine = BaseAllocator::MakeSharedPtr<AudioEngine>(this, mAudioEngine.get());
  mSerial->ReadEditorSoundsConfiguration(GetPathFromShared("/Configuration/Theme/Sound/EditorSounds.yml").value());
  CheckGLADIntegrity();

  mDefaults = BaseAllocator::MakeSharedPtr<DefaultValues>(this);
  mInterface = BaseAllocator::MakeSharedPtr<Interface>(mWindow.get(), this);
  SetupCamera();
  mEditorExplorer = BaseAllocator::MakeSharedPtr<EditorExplorer>(this);
  mPhysXHandle = BaseAllocator::MakeSharedPtr<PhysXHandle>(this);
  if (!mPhysXHandle->InitPxEngine()) {
    Yeager::Log(ERROR, "PhysX cannot initialize correctly, something must went wrong!");
  }
  mCommonTextOnScreen.Initialize();
  mCommonTextOnScreen.LoadFont(GetPathFromShared("/Resources/Fonts/Editor/firacode.ttf").value(), 48);
}

void ApplicationCore::SetupCamera()
{
  mEditorCamera = BaseAllocator::MakeSharedPtr<EditorCamera>(EntityBuilder(this));
  mBaseCamera = BaseAllocator::MakeSharedPtr<BaseCamera>(EntityBuilder(this));
  mBaseCamera = static_cast<std::shared_ptr<BaseCamera>>(mEditorCamera);
}

void ApplicationCore::UpdateCamera()
{
  if (mBaseCamera->GetCameraType() == YgCameraType::eCAMERA_PLAYER) {
    PlayerCamera* camera = static_cast<PlayerCamera*>(mBaseCamera.get());
    camera->Update(mDeltaTime);
  }
}

void ApplicationCore::Setup()
{
  BuildApplicationCoreCompoments();

  const std::optional<LauncherProjectPicker> project = RequestLauncher();

  if (project.has_value()) {

    mCurrentMode = ApplicationMode::eAPPLICATION_EDITOR;

    UVector2 wndSz(mWindow->GetWindowInformationPtr()->mEditorSize.x,
                   mWindow->GetWindowInformationPtr()->mEditorSize.y);

    mWindow->RegenerateMainWindow(wndSz, RequestWindowEngineName(project.value()));

    PrepareSceneToLoad(project.value());

  } else {
    mAudioEngine->TerminateAudioEngine();
    mSerial->WriteEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());
  }
}

void ApplicationCore::PrepareSceneToLoad(const LauncherProjectPicker& project)
{
  mScene->BuildScene(project);
  mSerial->ReadSceneShadersConfig(GetPathFromShared("/Configuration/Shader/DefaultShaders.yaml").value());
  mLauncher->GetNewProjectLoaded()
      ? mScene->Save()
      : mScene->Load(project.m_ProjectConfigurationPath);  // Check if project is new or already exists
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(mWindow->GetGLFWwindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  mSerial->WriteLoadedProjectsHandles(GetPathFromLocal("/Configuration/Projects").value());

  mSerial.reset();
  mBaseCamera.reset();
  mEditorCamera.reset();
  mEditorExplorer.reset();
  mAudioEngine.reset();
  mScene.reset();
  mDefaults.reset();
  mInterface.reset();
  mInput.reset();
  mWindow.reset();
  mSettings.reset();
}

void ApplicationCore::UpdateDeltaTime()
{
  mFrameCurrentCount++;
  const auto currentFrame = static_cast<float>(glfwGetTime());
  mDeltaTime = currentFrame - mLastFrame;
  mLastFrame = currentFrame;
}

void ApplicationCore::UpdateWorldMatrices()
{
  if (mSettings->GetVideoSettingsStruct().WindowObeysAspectRatio) {
    const float aspect = AspectRatio::ToValue(mSettings->GetVideoSettingsStruct().WindowDesireAspectRatio);
    mWorldMatrices.mProjection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
  } else {
    const Vector2 windowSize = mWindow->GetWindowInformationPtr()->mEditorSize;
    mWorldMatrices.mProjection = glm::perspective(glm::radians(45.0f), windowSize.x / windowSize.y, 0.1f, 1000.0f);
  }
  mWorldMatrices.mView = GetCamera()->ReturnViewMatrix();
  mWorldMatrices.mViewerPos = GetCamera()->GetPosition();
}

void ApplicationCore::UpdateListenerPosition()
{
  const irrklang::vec3df cameraPos = Yeager::GLMVec3ToVec3df(mWorldMatrices.mViewerPos);
  const irrklang::vec3df cameraDir = Yeager::GLMVec3ToVec3df(GetCamera()->GetDirection());
  mAudioEngine->SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                                irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::ShowCommonTextOnScreen()
{
  if (mScene->GetThreadAnimatedImporters()->size() + mScene->GetThreadImporters()->size() > 0) {
    mCommonTextOnScreen.RenderText(ShaderFromVarName("Font2D"), "Loading Assets", 0, 100,
                                   mSettings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
  }

  if ((unsigned int)GetSecondsElapsedSinceStart() > 0) {
    String fps = std::to_string(GetFrameCurrentCount() / (unsigned int)GetSecondsElapsedSinceStart());
    String t = String("FPS: " + fps);
    Yeager::WindowInfo* wnd = mWindow->GetWindowInformationPtr();
    mCommonTextOnScreen.RenderText(ShaderFromVarName("Font2D"), t, 0, 0,
                                   mSettings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
  }
}

void ApplicationCore::ProcessArgumentsDuringRender()
{
  if (mArguments.mVirtualMachine.bActivated) {
    if (GetSecondsElapsedSinceStart() > mArguments.mVirtualMachine.mTimer) {
      glfwSetWindowShouldClose(mWindow->GetGLFWwindow(), GLFW_TRUE);
    }
  }
}

void ApplicationCore::CreateGeneralLight()
{
  std::vector<Shader*> shaders = {ShaderFromVarName("Common"),
                                  ShaderFromVarName("Simple"),
                                  ShaderFromVarName("TerrainGeneration"),
                                  ShaderFromVarName("SimpleAnimated"),
                                  ShaderFromVarName("SimpleInstanced"),
                                  ShaderFromVarName("SimpleInstancedAnimated")};

  mGeneralLight = BaseAllocator::MakeSharedPtr<PhysicalLightHandle>(EntityBuilder(this, "General Light"), shaders,
                                                                    ShaderFromVarName("Light"));
  mGeneralLight->SetCanBeSerialize(false);
  mGeneralLight->GetDirectionalLight()->Ambient = Vector3(1);
  mScene->GetLightSources()->push_back(mGeneralLight);
}

void ApplicationCore::UpdateTheEngine()
{
  OpenGLFunc();
  CreateGeneralLight();
  BeginEngineTimer();

  while (ShouldRender()) {

    IntervalElapsedTimeManager::ResetIntervals();
    IntervalElapsedTimeManager::StartTimeInterval("Application Frame");

    ProcessArgumentsDuringRender();
    mWindow->StartFrame();
    OpenGLClear();

    mInterface->InitRenderFrame();
    mScene->CheckThreadsAndTriggerActions();

    UpdateDeltaTime();
    UpdateWorldMatrices();
    UpdateListenerPosition();
    ManifestAllShaders();
    UpdateCamera();

    mAudioEngine->Engine->update();

    mPhysXHandle->StartSimulation(mDeltaTime);
    mPhysXHandle->EndSimulation();

    DrawObjects();
    BuildAndDrawLightSources();

    mScene->DrawSkybox(ShaderFromVarName("Skybox"), mWorldMatrices.mView, mWorldMatrices.mProjection);

    mInterface->RenderUI();
    mScene->CheckScheduleDeletions();
    mInput->ProcessInputRender(mWindow.get(), mDeltaTime);
    mRequest->HandleRequests();

    IntervalElapsedTimeManager::EndTimeInterval("Application Frame");

    mInterface->DebugTimeInterval();
    mInterface->TerminateRenderFrame();
    mWindow->EndFrame();
  }

  TerminatePosRender();
}

void ApplicationCore::TerminatePosRender()
{
  mScene->CheckAndAwaitThreadsToFinish();
  mScene->Save();

  mAudioEngine->TerminateAudioEngine();
  mPhysXHandle->TerminateEngine();

  mSerial->WriteEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());

  mGeneralLight.reset();
  mPhysXHandle.reset();
  mScene->Terminate();
  mInterface->Terminate();
  mWindow->Terminate();
}

void ApplicationCore::BuildAndDrawLightSources()
{
  for (const auto& light : *GetScene()->GetLightSources()) {
    light->BuildShaderProps(GetCamera()->GetPosition(), GetCamera()->GetDirection(), 32.0f);
    light->DrawLightSources(mDeltaTime);
  }
}

void ApplicationCore::AttachPlayerCamera(std::shared_ptr<PlayerCamera> camera)
{
  camera->TransferInformation(mBaseCamera.get());
  mBaseCamera = static_cast<std::shared_ptr<BaseCamera>>(camera);
}

void ApplicationCore::DetachPlayerCamera()
{
  mEditorCamera->TransferInformation(mBaseCamera.get());
  mBaseCamera = static_cast<std::shared_ptr<BaseCamera>>(mEditorCamera);
}

void ApplicationCore::BeginEngineTimer()
{
  mTimeElapsedSinceStart = std::chrono::system_clock::now();
  mTimerHasStarted = true;
  mTimeBeforeRender = static_cast<float>(glfwGetTime());
}

float ApplicationCore::GetSecondsElapsedSinceStart() const
{
  if (mTimerHasStarted) {
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed = end - mTimeElapsedSinceStart;
    return elapsed.count();
  }
  return 1;
}

void ApplicationCore::DrawObjects()
{
  for (const auto& obj : *GetScene()->GetObjects()) {
    if (obj->IsInstanced()) {
      obj->Draw(ShaderFromVarName("SimpleInstanced"), mDeltaTime);
    } else {
      obj->Draw(ShaderFromVarName("Simple"), mDeltaTime);
    }
  }

  for (const auto& obj : *GetScene()->GetAnimatedObject()) {
    Shader* shader = YEAGER_NULLPTR;

    if (obj->IsInstanced()) {
      shader = ShaderFromVarName("SimpleInstancedAnimated");
    } else {
      shader = ShaderFromVarName("SimpleAnimated");
    }

    shader->UseShader();
    obj->UpdateAnimation(mDeltaTime);
    obj->BuildAnimationMatrices(shader);
    obj->Draw(shader);
  }
}

AudioEngine* ApplicationCore::GetAudioFromEngine()
{
  return mAudiosFromEngine.get();
}
Interface* ApplicationCore::GetInterface()
{
  return mInterface.get();
}
Input* ApplicationCore::GetInput()
{
  return mInput.get();
}
Window* ApplicationCore::GetWindow()
{
  return mWindow.get();
}
EditorExplorer* ApplicationCore::GetExplorer()
{
  return mEditorExplorer.get();
}
BaseCamera* ApplicationCore::GetCamera()
{
  return mBaseCamera.get();
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return mScene.get();
}

ApplicationMode::Enum ApplicationCore::GetMode() const noexcept
{
  return mCurrentMode;
}
ApplicationState::Enum ApplicationCore::GetState() const noexcept
{
  return mCurrentState;
}

void ApplicationCore::SetMode(ApplicationMode::Enum mode) noexcept
{
  mCurrentMode = mode;
}
void ApplicationCore::SetState(ApplicationState::Enum state) noexcept
{
  mCurrentState = state;
}

void ApplicationCore::ManifestShaderProps(Yeager::Shader* shader)
{
  shader->UseShader();
  shader->SetMat4("view", mWorldMatrices.mView);
  shader->SetMat4("projection", mWorldMatrices.mProjection);
  shader->SetVec3("viewPos", mWorldMatrices.mViewerPos);
}

void ApplicationCore::ManifestAllShaders()
{
  for (const auto& shader : mConfigShaders) {
    ManifestShaderProps(ShaderFromVarName(shader.second));
  }
}

void ApplicationCore::CheckGLADIntegrity()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Yeager::Log(ERROR, "Cannot load GLAD!");
    /* That is a big problem here, are you sure that OpenGL and the video drivers are installed correctly?*/
    PanicCrashReport(std::runtime_error("Cannot load GLAD!"));  // Panic!!
  }

#ifdef DEBUG_OPENGL_FUN

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Yeager::GLMessageDebugCallback, YEAGER_NULLPTR);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, YEAGER_NULLPTR, GL_TRUE);
  }

#endif
}

void Yeager::PanicCrashReport(const std::exception& exc)
{
  String p = GetPathFromLocal("/Logs/CrashReport").value();
  String time_point = TimePointType::CurrentTimeFormatToFileFormat();
  String path = String(p + YG_PS + time_point + "_CrashReport.txt");
  gGlobalConsole.SetLogString(ConsoleLogSender(exc.what()));
  DumpConsoleDataInFile(path);
  DisplayWarningPanicMessageBox(exc.what(), path);
  try {
    throw exc;
  } catch (std::exception exc) {
    exit(0);
  }
}

void ApplicationCore::OpenGLFunc()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void ApplicationCore::OpenGLClear()
{
  const Vector3 color = mInterface->GetOpenGLDebugClearColor();
  glClearColor(color.x, color.y, color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader* ApplicationCore::ShaderFromVarName(String var)
{
  for (const auto& shader : mConfigShaders) {
    /* If the var requested is the same as the shader var name, then it returns a pointer to that shader
    first = the shader point / second = the var string */
    if (var == shader.second) {
      return shader.first.get();
    }
  }
  /* Developer errors must be (in certain times) handle by asserts, this kind of errors cannot be release in the engine build !*/
  static_assert("Shader var name cannot been found in the configuration of the application!");
  /* Doesnt trigger warning in gcc anymore, this part of the code wont be reach anyway */
  return YEAGER_NULLPTR;
}

void ApplicationCore::AddConfigShader(std::shared_ptr<Yeager::Shader> shader, const String& var) noexcept
{
  mConfigShaders.push_back(std::pair<std::shared_ptr<Yeager::Shader>, String>(shader, var));
}

std::vector<LoadedProjectHandle>* ApplicationCore::GetLoadedProjectsHandles()
{
  return &mLoadedProjectsHandles;
}
Serialization* ApplicationCore::GetSerial()
{
  return mSerial.get();
}
Settings* ApplicationCore::GetSettings()
{
  return mSettings.get();
}
RequestHandle* ApplicationCore::GetRequestHandle()
{
  return mRequest.get();
}
DefaultValues* ApplicationCore::GetDefaults()
{
  return mDefaults.get();
}
PhysXHandle* ApplicationCore::GetPhysXHandle()
{
  return mPhysXHandle.get();
}
AudioEngineHandle* ApplicationCore::GetAudioEngineHandle()
{
  return mAudioEngine.get();
}
physx::PxController* ApplicationCore::GetController()
{
  return mController.get();
}
float ApplicationCore::GetTimeBeforeLoop() const
{
  return mTimeBeforeRender;
}
long long ApplicationCore::GetFrameCurrentCount() const
{
  return mFrameCurrentCount;
}
