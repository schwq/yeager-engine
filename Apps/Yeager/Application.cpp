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
  ValidatesExternalEngineFolder();
  m_Serial->ReadLoadedProjectsHandles(m_EngineExternalFolder);
  Setup();
}

YgString ApplicationCore::GetPathRelativeToExternalFolder(YgString path) const
{
  return m_EngineExternalFolder + path;
}

void ApplicationCore::ValidatesExternalEngineFolder()
{
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
      Yeager::Log(ERROR, "std::filesystem::create_directory cannot create the Engine external folder!");
    }
  }

  m_EngineExternalFolder = GetExternalFolderLocation() + osFolder + YG_PS + "External";
}

YgString ApplicationCore::GetExternalFolderLocation()
{
  return GetExternalFolderPath();
}

void ApplicationCore::CreateDirectoriesAndFiles()
{
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
  m_AudioEngine = new AudioEngineHandle();
  m_AudioEngine->InitAudioEngine();
  CheckGLAD();
  m_Interface = new Interface(m_Window, this);
  m_EditorCamera = new EditorCamera(this);
  m_EditorExplorer = new EditorExplorer(this);
  m_PhysXHandle = new PhysXHandle(this);
  if (!m_PhysXHandle->InitPxEngine()) {
    Yeager::Log(ERROR, "PhysX cannot initialize correctly, something must went wrong!");
  }
}

void ApplicationCore::Setup()
{
  YgString EditorVariablesPath = GetPath("/Configuration/Editor/editor_variables.yml");

  BuildApplicationCoreCompoments();

  LauncherProjectPicker project = RequestLauncher();
  if (!project.UserWantToExit) {

    m_mode = YgApplicationMode::eAPPLICATION_EDITOR;
    m_Window->RegenerateMainWindow(1920, 1080, RequestWindowEngineName(project), m_Input->MouseCallback);
    m_Interface->RequestRestartInterface(m_Window);
    m_Scene = new Yeager::Scene(project.m_Name, project.m_AuthorName, project.m_SceneType, project.m_ProjectFolderPath,
                                project.m_SceneRenderer, this);
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

void ApplicationCore::LoadSceneDefaultEntities() {}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_Window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  m_Serial->WriteLoadedProjectsHandles(m_EngineExternalFolder);
  delete m_Serial;
  delete m_EditorExplorer;
  delete m_EditorCamera;
  delete m_AudioEngine;
  delete m_Scene;
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
  m_AudioEngine->SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                                 irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::Render()
{

  OpenGLFunc();
  auto source = std::make_shared<PhysicalLightHandle>(
      "Main", this, std::vector<Shader*>{ShaderFromVarName("Simple"), ShaderFromVarName("SimpleAnimated")},
      ShaderFromVarName("Light"));
  ObjectPointLight light;
  Transformation trans;
  source->AddObjectPointLight(light, trans);
  GetScene()->GetLightSources()->push_back(source);

  GetScene()->CheckDuplicatesLightSources();

  physx::PxMaterial* material = m_PhysXHandle->GetPxPhysics()->createMaterial(1.0f, 1.0f, 1.0f);

  for (auto& sources : *m_Scene->GetLightSources()) {
    for (auto& obj : *sources->GetObjectPointLights()) {
      YgVector3 pos = obj.ObjSource->GetTransformationPtr()->position;
      m_PhysXHandle->GetGeometryHandle()->CreatePrimitiveSphere(YEAGER_PHYSX_RIGID_STATIC_BODY, *material,
                                                                physx::PxTransform(YgVector3ToPxVec3(pos)), 1);
    }
  }

  physx::PxRigidDynamic* actor = (physx::PxRigidDynamic*)m_PhysXHandle->GetGeometryHandle()->CreatePrimitiveSphere(
      YEAGER_PHYSX_RIGID_DYNAMIC_BODY, *material, physx::PxTransform(physx::PxVec3(3.0f, 1.0f, 4.0f)), 1);
  ObjectPointLight ll;
  m_Scene->GetLightSources()->at(0)->AddObjectPointLight(&ll, ObjectGeometryType::ESphere);

  m_Controller = m_PhysXHandle->GetCharacterController()->CreateController(physx::PxControllerShapeType::eCAPSULE);
  m_TimeBeforeRender = static_cast<float>(glfwGetTime());

  bool collisionFloor = false;

  while (ShouldRender()) {
    glfwPollEvents();
    OpenGLClear();

    ll.ObjSource->GetTransformationPtr()->position = Yeager::PxVec3ToYgVector3(actor->getGlobalPose().p);

    m_Interface->InitRenderFrame();
    m_Scene->CheckThreadsAndTriggerActions();

    UpdateDeltaTime();
    UpdateWorldMatrices();

    UpdateListenerPosition();

    ManifestShaderProps(ShaderFromVarName("Skybox"));

    m_AudioEngine->Engine->update();

    if (!collisionFloor) {
      PhysXCollisionDetection det = m_PhysXHandle->GetCharacterController()->Move(
          m_Controller, physx::PxVec3(0.0f, -0.1f, 0.0f), 0.01f, m_DeltaTime, NULL);
      if (det.bHasCollideBelow)
        collisionFloor = true;
    } else {
      PhysXCollisionDetection det = m_PhysXHandle->GetCharacterController()->Move(
          m_Controller, physx::PxVec3(0.0f, 0.0f, 0.0f), 0.01f, m_DeltaTime, NULL);
      if (!det.bHasCollideBelow) {
        collisionFloor = false;
      }
    }

    physx::PxExtendedVec3 pos = m_Controller->getPosition();
    m_EditorCamera->SetPosition(YgVector3(pos.x, pos.y, pos.z));

    m_PhysXHandle->StartSimulation(m_DeltaTime);
    m_PhysXHandle->EndSimulation();
    ManifestShaderProps(ShaderFromVarName("TerrainGeneration"));
    ManifestShaderProps(ShaderFromVarName("Simple"));
    ManifestShaderProps(ShaderFromVarName("Collision"));

    ManifestShaderProps(ShaderFromVarName("SimpleAnimated"));
    ManifestShaderProps(ShaderFromVarName("SimpleInstancedAnimated"));
    ShaderFromVarName("SimpleInstancedAnimated")->UseShader();

    DrawObjects();
    ManifestShaderProps(ShaderFromVarName("SimpleInstanced"));

    ManifestShaderProps(ShaderFromVarName("Light"));
    BuildAndDrawLightSources();

    GetInterface()->RenderUI();
    GetScene()->CheckScheduleDeletions();
    m_Interface->TerminateRenderFrame();
    GetInput()->ProcessInputRender(GetWindow(), m_DeltaTime);
    glfwSwapBuffers(GetWindow()->getWindow());
  }
  m_Controller->release();
  GetScene()->CheckAndAwaitThreadsToFinish();
  GetScene()->Save();
  m_AudioEngine->TerminateAudioEngine();
  m_PhysXHandle->TerminateEngine();
  delete m_PhysXHandle;
}

void ApplicationCore::BuildAndDrawLightSources()
{
  for (const auto& light : *GetScene()->GetLightSources()) {
    light->BuildShaderProps(GetCamera()->GetPosition(), GetCamera()->GetDirection(), 32.0f);
    light->DrawLightSources();
  }
}

void ApplicationCore::DrawObjects()
{
  for (const auto& obj : *GetScene()->GetObjects()) {
    obj->Draw(ShaderFromVarName("Simple"));
  }
  for (const auto& obj : *GetScene()->GetInstancedObjects()) {
    obj->Draw(ShaderFromVarName("SimpleInstanced"), obj->GetInstancedNumber());
  }

  for (const auto& obj : *GetScene()->GetAnimatedObject()) {
    ShaderFromVarName("SimpleAnimated")->UseShader();
    obj->UpdateAnimation(m_DeltaTime);
    obj->BuildAnimationMatrices(ShaderFromVarName("SimpleAnimated"));
    obj->Draw(ShaderFromVarName("SimpleAnimated"));
  }

  for (const auto& obj : *GetScene()->GetInstancedAnimatedObjects()) {
    ShaderFromVarName("SimpleInstancedAnimated")->UseShader();
    obj->UpdateAnimation(m_DeltaTime);
    obj->BuildAnimationMatrices(ShaderFromVarName("SimpleInstancedAnimated"));
    obj->Draw(ShaderFromVarName("SimpleInstancedAnimated"));
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
EditorCamera* ApplicationCore::GetCamera()
{
  return m_EditorCamera;
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_Scene;
}

YgApplicationMode::Enum ApplicationCore::GetMode() noexcept
{
  return m_mode;
}
YgApplicationState::Enum ApplicationCore::GetState() noexcept
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
  //glEnable(GL_CULL_FACE);
}

void ApplicationCore::OpenGLClear()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Shader* ApplicationCore::ShaderFromVarName(YgString var)
{
  for (const auto& shader : m_ConfigShaders) {
    /* If the var requested is the same as the shader var name, then it returns a pointer to that shader
    first = the shader point / second = the var string */
    if (var == shader.second) {
      return shader.first.get();
    }
  }
  /* Developer errors must be (in certain times) handle by asserts, this kind of errors cnanot be release in the engine build !*/
  assert("Shader var name cannot been found in the configuration of the application!");
  /* Doesnt trigger warning in gcc anymore, this part of the code wont be reach anyway */
  return nullptr;
}

void ApplicationCore::AddConfigShader(std::shared_ptr<Yeager::Shader> shader, YgString var) noexcept
{
  std::pair<std::shared_ptr<Yeager::Shader>, YgString> config;
  config.first = shader;
  config.second = var;
  m_ConfigShaders.push_back(config);
}