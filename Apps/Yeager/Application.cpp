#include "Application.h"
#include "Engine/Lighting/LightHandle.h"
#include "Engine/Renderer/Animation/AnimationEngine.h"
#include "Engine/Renderer/Object.h"
#include "Engine/Renderer/Skybox.h"
#include "Engine/Terrain/ProceduralTerrain.h"

using namespace Yeager;

ApplicationCore::ApplicationCore()
{
  m_Serial = new Serialization(this);
  m_Scene = new Scene(this);
  m_Window = new Window(this);
  ValidatesExternalEngineFolder();
  m_Serial->ReadLoadedProjectsHandles(m_EngineExternalFolder);
  Setup();
}

String ApplicationCore::GetPathRelativeToExternalFolder(String path) const
{
  return m_EngineExternalFolder + path;
}

void ApplicationCore::ValidatesExternalEngineFolder()
{
  String osFolder;
#ifdef YEAGER_SYSTEM_LINUX
  osFolder = "/.YeagerEngine";
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  osFolder = "\\YeagerEngine";
#endif

  if (!Yeager::ValidatesPath(GetExternalFolderLocation() + osFolder)) {
    Yeager::Log(WARNING, "Could not find the YeagerEngine external folder!");
    if (std::filesystem::create_directory(GetExternalFolderLocation() + osFolder)) {
      Yeager::Log(INFO, "Created the Engine external folder!");
      CreateDirectoriesAndFiles();
    } else {
      Yeager::Log(ERROR, "std::filesystem::create_directory cannot create the Engine external folder!");
    }
  }

  m_EngineExternalFolder = GetExternalFolderLocation() + osFolder + YG_PS + "External";
}

String ApplicationCore::GetExternalFolderLocation()
{
  return GetExternalFolderPath();
}

void ApplicationCore::CreateDirectoriesAndFiles()
{
  String osFolder;
#ifdef YEAGER_SYSTEM_LINUX
  osFolder = "/.YeagerEngine/External";
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
  osFolder = "\\YeagerEngine\\External";
#endif

  /** External folder for projects paths and information about the engine loaded in the OS */
  std::filesystem::create_directory(GetExternalFolderLocation() + osFolder);
  std::ofstream LoadedProjectsPathFile(GetExternalFolderLocation() + osFolder + "/LoadedProjectsPath.yml");
  if (LoadedProjectsPathFile.is_open()) {
    LoadedProjectsPathFile << "ProjectsLoaded: \n []";
  } else {
    Yeager::Log(ERROR, "Could not open the {}/LoadedProjectsPath.yml for writing!", osFolder);
  }
  LoadedProjectsPathFile.close();
}

LauncherProjectPicker ApplicationCore::RequestLauncher()
{
  const ImVec2 launcherWindowSize = m_Settings->GetEngineConfiguration()->LauncherWindowSize;
  m_Window->GetWindowInformationPtr()->LauncherSize = Vector2(launcherWindowSize.x, launcherWindowSize.y);
  m_Launcher = new Yeager::Launcher(launcherWindowSize.x, launcherWindowSize.y, "Yeager Launcher", this);
  m_Launcher->Render();

  if (ShouldRender()) {
    // The user have not decided to exit the program during the launcher phase
    return m_Launcher->GetSelectedProject();
  } else {
    // The user have requested a exit program, it returns a "fake" project with the (UserWantToExit) variable set to true
    m_Launcher->GetSelectedProjectPtr()->UserWantToExit = true;
    return m_Launcher->GetSelectedProject();
  }
}

String ApplicationCore::RequestWindowEngineName(const LauncherProjectPicker& project)
{
  String scene_renderer_str = Yeager::SceneRendererToString(project.m_SceneRenderer);
  std::replace(scene_renderer_str.begin(), scene_renderer_str.end(), '_', ' ');
  String engine_new_name = "Yeager Engine : " + project.m_Name + " / " + scene_renderer_str;
  return engine_new_name;
}

void ApplicationCore::BuildApplicationCoreCompoments()
{
  m_Settings = new Settings(this);
  m_Request = new RequestHandle(this);
  m_Input = new Input(this);

  m_EngineConfigurationPath = String(m_EngineExternalFolder + YG_PS + "Conf/EngineConfiguration.yml");
  m_Serial->ReadEngineConfiguration(m_EngineConfigurationPath);

  m_Window->GenerateWindow("Yeager Engine", m_Input->MouseCallback, YEAGER_GENERATE_LAUNCHER_WINDOW);
  m_Input->InitializeCallbacks();
  m_AudioEngine = new AudioEngineHandle();
  m_AudioEngine->InitAudioEngine();
  CheckGLAD();

  m_Defaults = new DefaultValues(this);
  m_Interface = new Interface(m_Window, this);
  SetupCamera();
  m_EditorExplorer = new EditorExplorer(this);
  m_PhysXHandle = new PhysXHandle(this);
  if (!m_PhysXHandle->InitPxEngine()) {
    Yeager::Log(ERROR, "PhysX cannot initialize correctly, something must went wrong!");
  }
}

void ApplicationCore::SetupCamera()
{
  m_EditorCamera = new EditorCamera(this);
  m_BaseCamera = static_cast<BaseCamera*>(m_EditorCamera);
}

void ApplicationCore::UpdateCamera()
{
  if (m_BaseCamera->GetCameraType() == YgCameraType::eCAMERA_PLAYER) {
    PlayerCamera* camera = static_cast<PlayerCamera*>(m_BaseCamera);
    camera->Update(m_DeltaTime);
  }
}

void ApplicationCore::Setup()
{

  BuildApplicationCoreCompoments();

  LauncherProjectPicker project = RequestLauncher();
  if (!project.UserWantToExit) {
    m_mode = YgApplicationMode::eAPPLICATION_EDITOR;
    const ImVec2 editorWindowSize =
        ImVec2(m_Window->GetWindowInformationPtr()->EditorSize.x, m_Window->GetWindowInformationPtr()->EditorSize.y);
    m_Window->GetWindowInformationPtr()->EditorSize = Vector2(editorWindowSize.x, editorWindowSize.y);
    m_Window->RegenerateMainWindow(editorWindowSize.x, editorWindowSize.y, RequestWindowEngineName(project),
                                   m_Input->MouseCallback);
    m_Interface->RequestRestartInterface(m_Window);

    PrepareSceneToLoad(project);

  } else {
    m_AudioEngine->TerminateAudioEngine();
  }
}

void ApplicationCore::PrepareSceneToLoad(const LauncherProjectPicker& project)
{
  m_Scene->BuildScene(project.m_Name, project.m_AuthorName, project.m_SceneType, project.m_ProjectFolderPath,
                      project.m_SceneRenderer, project.m_ProjectDateOfCreation);

  m_Serial->ReadSceneShadersConfig(GetPath("/Configuration/Editor/Shaders/default_shaders.yaml"));

  if (m_Launcher->GetNewProjectLaoded()) {
    /* New project have been loaded*/
    m_Scene->Save();

  } else {
    /* The project already exists! */
    m_Scene->Load(project.m_ProjectConfigurationPath);
  }
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_Window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  m_Serial->WriteLoadedProjectsHandles(m_EngineExternalFolder);
  YEAGER_DELETE(m_Serial);
  YEAGER_DELETE(m_EditorExplorer);
  YEAGER_DELETE(m_EditorCamera);
  YEAGER_DELETE(m_AudioEngine);
  YEAGER_DELETE(m_Scene);
  YEAGER_DELETE(m_Defaults)
  YEAGER_DELETE(m_Interface);
  YEAGER_DELETE(m_Input);
  YEAGER_DELETE(m_Window);
  YEAGER_DELETE(m_Settings);
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
  const irrklang::vec3df cameraPos = Yeager::YgVec3_to_Vec3df(m_WorldMatrices.ViewerPos);
  const irrklang::vec3df cameraDir = Yeager::YgVec3_to_Vec3df(GetCamera()->GetDirection());
  m_AudioEngine->SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                                 irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::Render()
{
  OpenGLFunc();

  m_TimeBeforeRender = static_cast<float>(glfwGetTime());

  auto light = std::make_shared<PhysicalLightHandle>(
      "main", this, std::vector<Shader*>{ShaderFromVarName("Simple"), ShaderFromVarName("SimpleAnimated")},
      ShaderFromVarName("Light"));
  light->SetCanBeSerialize(false);
  light->GetDirectionalLight()->Ambient = Vector3(1);
  m_Scene->GetLightSources()->push_back(light);

  BeginEngineTimer();

  auto skybox = std::make_shared<Yeager::Skybox>("Skybox", ObjectGeometryType::eCUSTOM, this);
  skybox->BuildSkyboxFromImport("/home/schwq/.YeagerEngine/External/Default/Skybox/skybox.obj", false);

  TextRenderer text(this);
  text.LoadFont(GetPath("/Assets/Fonts/firacode.ttf"), 48);
  Transformation trans;
  trans.position = Vector3(10);

  while (ShouldRender()) {

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
    skybox->Draw(ShaderFromVarName("Skybox"), Matrix3(m_WorldMatrices.View), m_WorldMatrices.Projection);

    m_PhysXHandle->StartSimulation(m_DeltaTime);
    m_PhysXHandle->EndSimulation();

    DrawObjects();
    BuildAndDrawLightSources();

    if (m_Scene->GetThreadAnimatedImporters()->size() + m_Scene->GetThreadImporters()->size() > 0) {
      text.RenderText(ShaderFromVarName("Font2D"), "Loading Assets", 0, 100,
                      m_Settings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
    }

    if ((unsigned int)GetSecondsElapsedSinceStart() > 0) {
      String fps = std::to_string(GetFrameCurrentCount() / (unsigned int)GetSecondsElapsedSinceStart());
      String t = String("FPS: " + fps);
      text.RenderText(ShaderFromVarName("Font2D"), t, 0, 0,
                      m_Settings->GetInterfaceSettingsStruct().GlobalOnScreenTextScale, Vector3(1));
    }

    GetInterface()->RenderUI();
    GetScene()->CheckScheduleDeletions();
    m_Interface->TerminateRenderFrame();
    GetInput()->ProcessInputRender(GetWindow(), m_DeltaTime);
    glfwSwapBuffers(GetWindow()->getWindow());
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

  m_Serial->WriteEngineConfiguration(m_EngineConfigurationPath);

  YEAGER_DELETE(m_PhysXHandle);
}

void ApplicationCore::BuildAndDrawLightSources()
{
  for (const auto& light : *GetScene()->GetLightSources()) {
    light->BuildShaderProps(GetCamera()->GetPosition(), GetCamera()->GetDirection(), 32.0f);
    light->DrawLightSources(m_DeltaTime);
  }
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

Interface* ApplicationCore::GetInterface()
{
  return m_Interface;
}
Input* ApplicationCore::GetInput()
{
  return m_Input;
}
Window* ApplicationCore::GetWindow()
{
  return m_Window;
}
EditorExplorer* ApplicationCore::GetExplorer()
{
  return m_EditorExplorer;
}
BaseCamera* ApplicationCore::GetCamera()
{
  return m_BaseCamera;
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_Scene;
}

YgApplicationMode::Enum ApplicationCore::GetMode() const noexcept
{
  return m_mode;
}
YgApplicationState::Enum ApplicationCore::GetState() const noexcept
{
  return m_state;
}

void ApplicationCore::SetMode(YgApplicationMode::Enum mode) noexcept
{
  m_mode = mode;
}
void ApplicationCore::SetState(YgApplicationState::Enum state) noexcept
{
  m_state = state;
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

void ApplicationCore::CheckGLAD()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Yeager::Log(ERROR, "Cannot load GLAD!");
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
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
  assert("Shader var name cannot been found in the configuration of the application!");
  /* Doesnt trigger warning in gcc anymore, this part of the code wont be reach anyway */
  return YEAGER_NULLPTR;
}

void ApplicationCore::AddConfigShader(std::shared_ptr<Yeager::Shader> shader, const String& var) noexcept
{
  std::pair<std::shared_ptr<Yeager::Shader>, String> config;
  config.first = shader;
  config.second = var;
  m_ConfigShaders.push_back(config);
}