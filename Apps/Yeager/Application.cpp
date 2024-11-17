#include "Application.h"
#include "Engine/Lighting/LightHandle.h"
#include "Engine/Renderer/Animation/AnimationEngine.h"
#include "Engine/Renderer/Object.h"
#include "Engine/Renderer/Skybox.h"
#include "Engine/Terrain/ProceduralTerrain.h"

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
    if (proj.Name == ForcedEntry.ProjectName) {
      LauncherProjectPicker project(proj);
      return project;
    }
  }
  return std::nullopt;
}

ApplicationCore::ApplicationCore(int argc, char* argv[])
    : m_Serial(std::make_shared<Serialization>(this)),
      m_Scene(std::make_shared<Scene>(this)),
      m_Window(std::make_shared<Window>(this)),
      m_CommonTextOnScreen(this),
      mCurrentLocale(this, ELanguangeRegion::EN_US, true)
{
  InitializeRandomGenerator();
  ProcessArguments(argc, argv);
  ValidatesExternalEngineFolder();
  m_Serial->ReadLoadedProjectsHandles(GetPathFromLocal("/Configuration/Projects").value());
  Setup();
}

std::optional<const String> ApplicationCore::AcessableArgumentArray(Uint index)
{
  if (index > m_AllArguments.size())
    return std::nullopt;
  return m_AllArguments.at(index);
}

void ApplicationCore::ProcessArguments(int argc, char* argv[])
{

  if (argc > 1) {
    m_AllArguments.assign(argv + 1, argv + argc);

    Uint index = 0;
    while (index < m_AllArguments.size()) {
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
      m_Arguments.ForcedEntry.Activated = true;
      m_Arguments.ForcedEntry.ProjectName = entry.value();
      return 2;
    }
    case StringToInterger("-DebugTitleString"): {

      std::optional<const String> title = AcessableArgumentArray(index + 1);
      if (!title.has_value()) {
        Yeager::Log(ERROR, "Cannot process arguments properly, trying to access array from undefined index!");
        return 1;
      }

      m_Arguments.DebugTitle.Activated = true;
      m_Arguments.DebugTitle.Title = title.value();
      return 2;
    }
    case StringToInterger("-DebugVirtualMachine"): {
      std::optional<const String> vm = AcessableArgumentArray(index + 1);
      if (vm.has_value() && vm.value() == "valgrind") {
        Yeager::Log(INFO, "Current debugging virtual machine is set to VALGRIND!");
        m_Arguments.VirtualMachine.Activated = true;
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
        m_Arguments.StartupNetwork.IpAddr = ip.value();
        m_Arguments.StartupNetwork.Port = std::stoi(port.value());
        m_Arguments.StartupNetwork.Role = NetworkCommunicationRole::CLIENT;
        m_Arguments.StartupNetwork.Start();
        m_Arguments.StartupNetwork.Socket.Send("Hello from Yeager Engine!");
        std::vector<unsigned int> v = {1, 2, 3, 4, 5};
        m_Arguments.StartupNetwork.Socket.Send(v);
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
  if (m_Arguments.DebugTitle.Activated) {
    title += " " + m_Arguments.DebugTitle.Title;
  }

  const ImVec2 launcherWindowSize = m_Settings->GetEngineConfiguration()->LauncherWindowSize;
  m_Window->GetWindowInformationPtr()->LauncherSize = Vector2(launcherWindowSize.x, launcherWindowSize.y);
  m_Launcher = std::make_shared<Yeager::Launcher>(launcherWindowSize.x, launcherWindowSize.y, title, this);

  if (m_Arguments.ForcedEntry.Activated) {
    std::optional<Yeager::LauncherProjectPicker> project = m_Arguments.SearchForcedEntryProject(this);
    if (project.has_value()) {
      m_Interface->SetSceneFileText(Yeager::FileContentToString(project.value().m_ProjectConfigurationPath));
      return project.value();
    }
    Yeager::Log(ERROR, "Cannot find project in configuration file: {}", m_Arguments.ForcedEntry.ProjectName);
  }

  m_Launcher->Render();

  if (ShouldRender()) {
    // The user have not decided to exit the program during the launcher phase
    return m_Launcher->GetSelectedProject();
  } else {
    // The user have requested a exit program, it returns a "fake" project with the (UserWantToExit) variable set to true
    m_Launcher->GetSelectedProjectPtr()->UserWantToExit = true;
    return std::nullopt;
  }
}

String ApplicationCore::RequestWindowEngineName(const LauncherProjectPicker& project)
{
  String scene_renderer_str = Yeager::SceneRendererToString(project.m_SceneRenderer);
  std::replace(scene_renderer_str.begin(), scene_renderer_str.end(), '_', ' ');
  String engine_new_name = "Yeager Engine : " + project.m_Name + " / " + scene_renderer_str;
  if (m_Arguments.DebugTitle.Activated)
    engine_new_name += " " + m_Arguments.DebugTitle.Title;
  return engine_new_name;
}

void ApplicationCore::BuildApplicationCoreCompoments()
{
  m_Settings = std::make_shared<Settings>(this);
  m_Request = std::make_shared<RequestHandle>(this);
  m_Input = std::make_shared<Input>(this);

  m_Serial->ReadEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());

  m_Window->GenerateWindow("Yeager Engine", m_Input->MouseCallback, YEAGER_GENERATE_LAUNCHER_WINDOW);
  m_Input->InitializeCallbacks();
  m_AudioEngine = std::make_shared<AudioEngineHandle>();
  m_AudioEngine->InitAudioEngine();
  m_AudiosFromEngine = std::make_shared<AudioEngine>(this, m_AudioEngine.get());
  m_Serial->ReadEditorSoundsConfiguration(GetPathFromShared("/Configuration/Theme/Sound/EditorSounds.yml").value());
  CheckGLADIntegrity();

  m_Defaults = std::make_shared<DefaultValues>(this);
  m_Interface = std::make_shared<Interface>(m_Window.get(), this);
  SetupCamera();
  m_EditorExplorer = std::make_shared<EditorExplorer>(this);
  m_PhysXHandle = std::make_shared<PhysXHandle>(this);
  if (!m_PhysXHandle->InitPxEngine()) {
    Yeager::Log(ERROR, "PhysX cannot initialize correctly, something must went wrong!");
  }
  m_CommonTextOnScreen.Initialize();
  m_CommonTextOnScreen.LoadFont(GetPathFromShared("/Resources/Fonts/Editor/firacode.ttf").value(), 48);
}

void ApplicationCore::SetupCamera()
{
  m_EditorCamera = std::make_shared<EditorCamera>(EntityBuilder(this));
  m_BaseCamera = std::make_shared<BaseCamera>(EntityBuilder(this));
  m_BaseCamera = static_cast<std::shared_ptr<BaseCamera>>(m_EditorCamera);
}

void ApplicationCore::UpdateCamera()
{
  if (m_BaseCamera->GetCameraType() == YgCameraType::eCAMERA_PLAYER) {
    PlayerCamera* camera = static_cast<PlayerCamera*>(m_BaseCamera.get());
    camera->Update(m_DeltaTime);
  }
}

void ApplicationCore::Setup()
{
  BuildApplicationCoreCompoments();

  const std::optional<LauncherProjectPicker> project = RequestLauncher();

  if (project.has_value()) {

    m_CurrentMode = ApplicationMode::eAPPLICATION_EDITOR;
    m_Window->RegenerateMainWindow(m_Window->GetWindowInformationPtr()->EditorSize.x,
                                   m_Window->GetWindowInformationPtr()->EditorSize.y,
                                   RequestWindowEngineName(project.value()), m_Input->MouseCallback);
    m_Input->InitializeCallbacks();
    m_Interface->RequestRestartInterface(m_Window.get());

    PrepareSceneToLoad(project.value());

  } else {
    m_AudioEngine->TerminateAudioEngine();
    m_Serial->WriteEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());
  }
}

void ApplicationCore::PrepareSceneToLoad(const LauncherProjectPicker& project)
{
  m_Scene->BuildScene(project);
  m_Serial->ReadSceneShadersConfig(GetPathFromShared("/Configuration/Shader/default_shaders.yaml").value());
  m_Launcher->GetNewProjectLoaded()
      ? m_Scene->Save()
      : m_Scene->Load(project.m_ProjectConfigurationPath);  // Check if project is new or already exists
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_Window->GetGLFWwindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  m_Serial->WriteLoadedProjectsHandles(GetPathFromLocal("/Configuration/Projects").value());
  m_Serial.reset();

  m_BaseCamera = YEAGER_NULLPTR;
  m_BaseCamera.reset();
  m_EditorCamera.reset();

  m_EditorExplorer.reset();
  m_AudioEngine.reset();
  m_Scene.reset();
  m_Defaults.reset();
  m_Interface.reset();
  m_Input.reset();
  m_Window.reset();
  m_Settings.reset();
}

void ApplicationCore::UpdateDeltaTime()
{
  m_FrameCurrentCount++;
  const auto currentFrame = static_cast<float>(glfwGetTime());
  m_DeltaTime = currentFrame - m_LastFrame;
  m_LastFrame = currentFrame;
}

void ApplicationCore::UpdateWorldMatrices()
{
  if (m_Settings->GetVideoSettingsStruct().WindowObeysAspectRatio) {
    const float aspect = AspectRatio::ToValue(m_Settings->GetVideoSettingsStruct().WindowDesireAspectRatio);
    m_WorldMatrices.Projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
  } else {
    const Vector2 windowSize = m_Window->GetWindowInformationPtr()->EditorSize;
    m_WorldMatrices.Projection = glm::perspective(glm::radians(45.0f), windowSize.x / windowSize.y, 0.1f, 1000.0f);
  }
  m_WorldMatrices.View = GetCamera()->ReturnViewMatrix();
  m_WorldMatrices.ViewerPos = GetCamera()->GetPosition();
}

void ApplicationCore::UpdateListenerPosition()
{
  const irrklang::vec3df cameraPos = Yeager::GLMVec3ToVec3df(m_WorldMatrices.ViewerPos);
  const irrklang::vec3df cameraDir = Yeager::GLMVec3ToVec3df(GetCamera()->GetDirection());
  m_AudioEngine->SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                                 irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::ShowCommonTextOnScreen()
{
  if (m_Scene->GetThreadAnimatedImporters()->size() + m_Scene->GetThreadImporters()->size() > 0) {
    m_CommonTextOnScreen.RenderText(ShaderFromVarName("Font2D"), "Loading Assets", 0, 100,
                                    m_Settings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
  }

  if ((unsigned int)GetSecondsElapsedSinceStart() > 0) {
    String fps = std::to_string(GetFrameCurrentCount() / (unsigned int)GetSecondsElapsedSinceStart());
    String t = String("FPS: " + fps);
    Yeager::WindowInfo* wnd = m_Window->GetWindowInformationPtr();
    m_CommonTextOnScreen.RenderText(ShaderFromVarName("Font2D"), t, 0, 0,
                                    m_Settings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
  }
}

void ApplicationCore::ProcessArgumentsDuringRender()
{
  if (m_Arguments.VirtualMachine.Activated) {
    if (GetSecondsElapsedSinceStart() > m_Arguments.VirtualMachine.Timer) {
      glfwSetWindowShouldClose(m_Window->GetGLFWwindow(), GLFW_TRUE);
    }
  }
}

void ApplicationCore::UpdateTheEngine()
{
  OpenGLFunc();

  m_TimeBeforeRender = static_cast<float>(glfwGetTime());

  auto light = std::make_shared<PhysicalLightHandle>(
      EntityBuilder(this, "main"),
      std::vector<Shader*>{ShaderFromVarName("Simple"), ShaderFromVarName("SimpleAnimated")},
      ShaderFromVarName("Light"));
  light->SetCanBeSerialize(false);
  light->GetDirectionalLight()->Ambient = Vector3(1);
  m_Scene->GetLightSources()->push_back(light);

  BeginEngineTimer();
  while (ShouldRender()) {
    ProcessArgumentsDuringRender();
    glfwPollEvents();
    OpenGLClear();

    m_Interface->InitRenderFrame();
    m_Scene->CheckThreadsAndTriggerActions();

    UpdateDeltaTime();
    UpdateWorldMatrices();
    UpdateListenerPosition();
    ManifestAllShaders();
    UpdateCamera();

    m_AudioEngine->Engine->update();

    m_PhysXHandle->StartSimulation(m_DeltaTime);
    m_PhysXHandle->EndSimulation();

    DrawObjects();
    BuildAndDrawLightSources();

    m_Scene->DrawSkybox(ShaderFromVarName("Skybox"), m_WorldMatrices.View, m_WorldMatrices.Projection);

    GetInterface()->RenderUI();
    GetScene()->CheckScheduleDeletions();
    m_Interface->TerminateRenderFrame();
    GetInput()->ProcessInputRender(GetWindow(), m_DeltaTime);
    glfwSwapBuffers(GetWindow()->GetGLFWwindow());
    m_Request->HandleRequests();
  }

  TerminatePosRender();
}

void ApplicationCore::TerminatePosRender()
{
  GetScene()->CheckAndAwaitThreadsToFinish();
  GetScene()->Save();

  m_AudioEngine->TerminateAudioEngine();
  m_PhysXHandle->TerminateEngine();

  m_Serial->WriteEngineConfiguration(GetPathFromLocal("/Configuration/Variables/EngineConfiguration.yml").value());

  m_PhysXHandle.reset();
  m_Scene->Terminate();
  m_Interface->Terminate();
  m_Window->Terminate();
}

void ApplicationCore::BuildAndDrawLightSources()
{
  for (const auto& light : *GetScene()->GetLightSources()) {
    light->BuildShaderProps(GetCamera()->GetPosition(), GetCamera()->GetDirection(), 32.0f);
    light->DrawLightSources(m_DeltaTime);
  }
}

void ApplicationCore::AttachPlayerCamera(std::shared_ptr<PlayerCamera> camera)
{
  camera->TransferInformation(m_BaseCamera.get());
  m_BaseCamera = static_cast<std::shared_ptr<BaseCamera>>(camera);
}

void ApplicationCore::DetachPlayerCamera()
{
  m_EditorCamera->TransferInformation(m_BaseCamera.get());
  m_BaseCamera = static_cast<std::shared_ptr<BaseCamera>>(m_EditorCamera);
}

void ApplicationCore::BeginEngineTimer()
{
  m_TimeElapsedSinceStart = std::chrono::system_clock::now();
  m_TimerHasStarted = true;
}

float ApplicationCore::GetSecondsElapsedSinceStart() const
{
  if (m_TimerHasStarted) {
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed = end - m_TimeElapsedSinceStart;
    return elapsed.count();
  }
  return 1;
}

void ApplicationCore::DrawObjects()
{
  for (const auto& obj : *GetScene()->GetObjects()) {
    if (obj->IsInstanced()) {
      obj->Draw(ShaderFromVarName("SimpleInstanced"), m_DeltaTime);
    } else {
      obj->Draw(ShaderFromVarName("Simple"), m_DeltaTime);
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
    obj->UpdateAnimation(m_DeltaTime);
    obj->BuildAnimationMatrices(shader);
    obj->Draw(shader);
  }
}

AudioEngine* ApplicationCore::GetAudioFromEngine()
{
  return m_AudiosFromEngine.get();
}
Interface* ApplicationCore::GetInterface()
{
  return m_Interface.get();
}
Input* ApplicationCore::GetInput()
{
  return m_Input.get();
}
Window* ApplicationCore::GetWindow()
{
  return m_Window.get();
}
EditorExplorer* ApplicationCore::GetExplorer()
{
  return m_EditorExplorer.get();
}
BaseCamera* ApplicationCore::GetCamera()
{
  return m_BaseCamera.get();
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_Scene.get();
}

ApplicationMode::Enum ApplicationCore::GetMode() const noexcept
{
  return m_CurrentMode;
}
ApplicationState::Enum ApplicationCore::GetState() const noexcept
{
  return m_CurrentState;
}

void ApplicationCore::SetMode(ApplicationMode::Enum mode) noexcept
{
  m_CurrentMode = mode;
}
void ApplicationCore::SetState(ApplicationState::Enum state) noexcept
{
  m_CurrentState = state;
}

void ApplicationCore::ManifestShaderProps(Yeager::Shader* shader)
{
  shader->UseShader();
  shader->SetMat4("view", m_WorldMatrices.View);
  shader->SetMat4("projection", m_WorldMatrices.Projection);
  shader->SetVec3("viewPos", m_WorldMatrices.ViewerPos);
}

void ApplicationCore::ManifestAllShaders()
{
  for (const auto& shader : m_ConfigShaders) {
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
  const Vector3 color = m_Interface->GetOpenGLDebugClearColor();
  glClearColor(color.x, color.y, color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader* ApplicationCore::ShaderFromVarName(String var)
{
  for (const auto& shader : m_ConfigShaders) {
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
  std::pair<std::shared_ptr<Yeager::Shader>, String> config(shader, var);
  m_ConfigShaders.push_back(config);
}

std::vector<LoadedProjectHandle>* ApplicationCore::GetLoadedProjectsHandles()
{
  return &m_LoadedProjectsHandles;
}
Serialization* ApplicationCore::GetSerial()
{
  return m_Serial.get();
}
Settings* ApplicationCore::GetSettings()
{
  return m_Settings.get();
}
RequestHandle* ApplicationCore::GetRequestHandle()
{
  return m_Request.get();
}
DefaultValues* ApplicationCore::GetDefaults()
{
  return m_Defaults.get();
}
PhysXHandle* ApplicationCore::GetPhysXHandle()
{
  return m_PhysXHandle.get();
}
AudioEngineHandle* ApplicationCore::GetAudioEngineHandle()
{
  return m_AudioEngine.get();
}
physx::PxController* ApplicationCore::GetController()
{
  return m_Controller.get();
}
float ApplicationCore::GetTimeBeforeLoop() const
{
  return m_TimeBeforeRender;
}
long long ApplicationCore::GetFrameCurrentCount() const
{
  return m_FrameCurrentCount;
}
