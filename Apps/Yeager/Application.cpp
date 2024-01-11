#include "Application.h"
#include "Common/LogEngine.h"
#include "InputHandle.h"

Application::Application()
{
  Yeager::Log(INFO, kSystem, "Start program");
=======
  ValidatesExternalEngineFolder();

  m_Serial = new Serialization(this);
  m_Serial->ReadLoadedProjectsHandles(m_EngineExternalFolder);
  
  Setup();
>>>>>>> Stashed changes
}

Application::~Application()
{

  Yeager::Log(INFO, kSystem, "Destrorying Application!");
}

bool Application::EnterKeyPressed()
{
  return glfwGetKey(m_window->getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS;
}

bool Application::ShouldRendererActive()
{
  return (glfwWindowShouldClose(m_window->getWindow())) ? false : true;
}

void Application::SetupApplication(ApplicationSetup setup)
{
  m_camera = setup.ptr_camera;
  m_window = setup.ptr_window;
  m_engine = setup.ptr_engine;
  m_input = setup.ptr_input;
  m_interface = setup.ptr_interface;
  m_explorer = setup.ptr_explorer;
}
YgString osFolder;
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
      Yeager::Log(ERROR,
                  "std::filesystem::create_directory cannot create the Engine external folder!");
    }
  }

  m_EngineExternalFolder = GetExternalFolderLocation() + osFolder + YG_PS + "External";
  }


const void Application::SetCurrentMode(ApplicationCurrentMode mode)
{
  switch (mode) {
    case ApplicationCurrentMode::kEditorMode:
      m_input->SetCurrentMode(InputCurrentMode::kEditorMode);
      m_interface->SetCurrentMode(InterfaceMode::kEditorMode);
      break;
    case ApplicationCurrentMode::kAwaitMode:
      m_input->SetCurrentMode(InputCurrentMode::kAwaitMode);
      m_interface->SetCurrentMode(InterfaceMode::kAwaitMode);
      break;
    case ApplicationCurrentMode::kLauncherMode:
      m_input->SetCurrentMode(InputCurrentMode::kLauncherMode);
      m_interface->SetCurrentMode(InterfaceMode::kLauncherMode);
      break;
    case ApplicationCurrentMode::kSettingsMode:
      m_input->SetCurrentMode(InputCurrentMode::kSettingsMode);
      m_interface->SetCurrentMode(InterfaceMode::kSettingsMode);
      break;
    case ApplicationCurrentMode::kErrorMode:
      m_input->SetCurrentMode(InputCurrentMode::kErrorMode);
      m_interface->SetCurrentMode(InterfaceMode::kErrorMode);
      break;
    case ApplicationCurrentMode::kCeaseMode:
      m_input->SetCurrentMode(InputCurrentMode::kCeaseMode);
      m_interface->SetCurrentMode(InterfaceMode::kCeaseMode);
  }
}
=======
  YgString osFolder;
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
  m_Launcher = new Yeager::Launcher(800, 800, "Yeager Launcher", this);
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

YgString ApplicationCore::RequestWindowEngineName(const LauncherProjectPicker& project)
{
  YgString scene_renderer_str = Yeager::SceneRendererToString(project.m_SceneRenderer);
  std::replace(scene_renderer_str.begin(), scene_renderer_str.end(), '_', ' ');
  YgString engine_new_name = "Yeager Engine : " + project.m_Name + " / " + scene_renderer_str;
  return engine_new_name;
}

void ApplicationCore::BuildApplicationCoreCompoments()
{
  m_Input = new Input(this);
  m_Window = new Window(ygWindowWidth, ygWindowHeight, "Yeager Engine", m_Input->MouseCallback);
  m_Input->InitializeCallbacks();
  CheckGLAD();
  m_Interface = new Interface(m_Window, this);
  m_EditorCamera = new EditorCamera(this);
  m_EditorExplorer = new EditorExplorer(this);
}

void ApplicationCore::Setup()
{
  YgString EditorVariablesPath = GetPath("/Configuration/Editor/editor_variables.yml");

  BuildApplicationCoreCompoments();
  m_AudioEngine = new AudioEngineHandle;
  m_AudioEngine->InitAudioEngine();

  LauncherProjectPicker project = RequestLauncher();
  if (!project.UserWantToExit) {

    m_mode = Yeager::AppEditor;
    m_Window->RegenerateMainWindow(1920, 1080, RequestWindowEngineName(project), m_Input->MouseCallback);
    m_Interface->RequestRestartInterface(m_Window);
    m_Scene = new Yeager::Scene(project.m_Name, project.m_AuthorName, project.m_SceneType, project.m_ProjectFolderPath,
                                project.m_SceneRenderer, this);
    m_Serial = new Serialization(this);
    m_Serial->ReadEditorVariables(EditorVariablesPath.c_str());
    m_Serial->ReadSceneShadersConfig(GetPath("/Configuration/Editor/Shaders/default_shaders.yaml")); 

    if (m_Launcher->GetNewProjectLaoded()) {
      /* New project have been loaded*/
      m_Scene->Save();
      LoadSceneDefaultEntities();
    } else {
      /* The project already exists! */
      m_Scene->Load(project.m_ProjectConfigurationPath);
    }

  } else {
    m_AudioEngine->TerminateAudioEngine();
  }
}

void ApplicationCore::LoadSceneDefaultEntities()
{
  //GetScene()->GetCurrentSkybox()->BuildSkyboxFromImport(GetExternalFolder() + "/Default/Skybox/Skybox.obj");
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_Window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  m_Serial->WriteLoadedProjectsHandles(m_EngineExternalFolder);
  delete m_AudioEngine;
  delete m_EditorExplorer;
  delete m_EditorCamera;
  delete m_Scene;
  delete m_Serial;
  delete m_Interface;
  delete m_Input;
  delete m_Window;
}

void ApplicationCore::UpdateDeltaTime()
{
  m_FrameCurrentCount++;
  auto currentFrame = static_cast<float>(glfwGetTime());
  m_DeltaTime = currentFrame - m_LastFrame;
  m_LastFrame = currentFrame;
}

void ApplicationCore::UpdateWorldMatrices()
{
  m_WorldMatrices.Projection =
      glm::perspective(glm::radians(45.0f), (float)ygWindowWidth / (float)ygWindowHeight, 0.1f, 1000.0f);
  m_WorldMatrices.View = GetCamera()->ReturnViewMatrix();
  m_WorldMatrices.ViewerPos = GetCamera()->GetPosition();
}

void ApplicationCore::UpdateListenerPosition()
{
  irrklang::vec3df cameraPos = Yeager::YgVec3_to_Vec3df(m_WorldMatrices.ViewerPos);
  irrklang::vec3df cameraDir = Yeager::YgVec3_to_Vec3df(GetCamera()->GetDirection());
  m_AudioEngine->SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f), irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::Render()
{
  OpenGLFunc();

  while (ShouldRender()) {
    glfwPollEvents();
    OpenGLClear();

    GetInterface()->InitRenderFrame();
    m_Scene->CheckThreadsAndTriggerActions();

    UpdateDeltaTime();
    UpdateWorldMatrices();
    ManifestAllShaders();

    for (const auto& light : *GetScene()->GetLightSources()) {
      light->BuildShaderProps(GetCamera()->GetPosition(), GetCamera()->GetDirection(), 32.0f);
    }

    UpdateListenerPosition();

    m_AudioEngine->Engine->update();
          
    VerifyCollisions();
    DrawObjects();
   
    BuildAndDrawLightSources();

    GetInterface()->RenderUI();
    GetInterface()->TerminateRenderFrame();
    GetInput()->ProcessInputRender(GetWindow(), m_DeltaTime);
    glfwSwapBuffers(GetWindow()->getWindow());
  }

  GetScene()->Save();
  m_AudioEngine->TerminateAudioEngine();
}

void ApplicationCore::ManifestAllShaders() {
    for (const auto& shader : m_ConfigShaders) {
    ManifestShaderProps(ShaderFromVarName(shader.second));
  }
}

void ApplicationCore::VerifyCollisions()
{
  if (GetScene()->GetObjects()->empty() || GetScene()->GetObjects()->size() == 1) {
    return;
  }
  /* Make so the collision isnt tested against the object itself*/
  int objectsAlreadyChecked = 1;

  /* Resets collisions for all objects */
  for (const auto& obj : *GetScene()->GetObjects()) {
    obj->ForceCollision(false);
  }
    
  for (unsigned int x = 0; x < GetScene()->GetObjects()->size(); x++) {
    for (unsigned int y = objectsAlreadyChecked; y < GetScene()->GetObjects()->size(); y++) {
      Object* obj1 = GetScene()->GetObjects()->at(x).get();
      Object* obj2 = GetScene()->GetObjects()->at(y).get();
      if (obj1->GetCollision()->GetEnabled() && obj2->GetCollision()->GetEnabled()) {
        bool collision = obj1->GetCollision()->CheckCollision(obj2);
        obj1->MakeCollision(collision);
        obj2->MakeCollision(collision);
      }
    }
    objectsAlreadyChecked++;
  }
}

void ApplicationCore::BuildAndDrawLightSources()
{
  for (const auto& light : *GetScene()->GetLightSources()) {
    light->DrawLightSources();
  }
}

void ApplicationCore::DrawObjects()
{
  ShaderFromVarName("Simple")->UseShader();
  for (const auto& obj : *GetScene()->GetObjects()) {
    if (obj->IsLoaded()) {
        obj->Draw(ShaderFromVarName("Simple"));
    }
  }
  ShaderFromVarName("SimpleInstanced")->UseShader();
  for (const auto& obj : *GetScene()->GetInstancedObjects()) {
    if (obj->IsLoaded()) {
        obj->Draw(ShaderFromVarName("SimpleInstanced"), obj->GetInstancedNumber());
    }
  }

  ShaderFromVarName("SimpleAnimated")->UseShader();
  for (const auto& obj : *GetScene()->GetAnimatedObject()) {
    ShaderFromVarName("SimpleAnimated")->UseShader();
    if (obj->IsLoaded()) {
        obj->UpdateAnimation(m_DeltaTime);
        obj->BuildAnimationMatrices(ShaderFromVarName("SimpleAnimated"));
        obj->Draw(ShaderFromVarName("SimpleAnimated"));
    }
    }

  ShaderFromVarName("SimpleInstancedAnimated")->UseShader();
  for (const auto& obj : *GetScene()->GetInstancedAnimatedObjects()) {
    ShaderFromVarName("SimpleInstancedAnimated")->UseShader();
    if (obj->IsLoaded()) {
        obj->UpdateAnimation(m_DeltaTime);
        obj->BuildAnimationMatrices(ShaderFromVarName("SimpleInstancedAnimated"));
        obj->Draw(ShaderFromVarName("SimpleInstancedAnimated"));
    }
    }
}

void ApplicationCore::SetCurrentShaderInUse(Shader* shader) {
    m_CurrentShaderInUse = shader;
}
Shader* ApplicationCore::GetCurrentShaderInUse() {
    return m_CurrentShaderInUse;
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
EditorCamera* ApplicationCore::GetCamera()
{
  return m_EditorCamera;
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_Scene;
}

ApplicationMode ApplicationCore::GetMode() noexcept
{
  return m_mode;
}
ApplicationState ApplicationCore::GetState() noexcept
{
  return m_state;
}

void ApplicationCore::SetMode(ApplicationMode mode) noexcept
{
  m_mode = mode;
}
void ApplicationCore::SetState(ApplicationState state) noexcept
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
  //glEnable(GL_CULL_FACE);
}

void ApplicationCore::OpenGLClear()
{
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Yeager::Shader* ApplicationCore::ShaderFromVarName(YgString var) {
    for (const auto& shader : m_ConfigShaders) {
    /* If the var requested is the same as the shader var name, then it returns a pointer to that shader
    first = the shader point / second = the var string */
        if (var == shader.second) {
      return shader.first.get();
    }
  }
    /* Developer errors must be (in certain times) handle by asserts, this kind of errors cnanot be release in the engine build !*/
    assert("Shader var name cannot been found in the configuration of the application!");
}

void ApplicationCore::AddConfigShader(std::shared_ptr<Yeager::Shader> shader, YgString var) noexcept {
    std::pair<std::shared_ptr<Yeager::Shader>, YgString> config;
    config.first = shader;
    config.second = var;
    m_ConfigShaders.push_back(config);
}

void ApplicationCore::ShaderHotReload(Yeager::Shader* shader) {
    shader->RequestHotReload();
}
>>>>>>> Stashed changes
