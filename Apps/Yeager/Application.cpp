#include "Application.h"
#include "Engine/Renderer/Animation/AnimationEngine.h"
#include "Engine/Renderer/Object.h"
#include "Engine/Renderer/Skybox.h"
#include "Engine/Terrain/ProceduralTerrain.h"
#include "Engine/Lighting/LightHandle.h"

using namespace Yeager;

ApplicationCore::ApplicationCore()
{
  Setup();
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
  CheckGLAD();
  m_Interface = new Interface(m_Window, this);
  m_EditorCamera = new EditorCamera(this);
  m_EditorExplorer = new EditorExplorer(this);
}

void ApplicationCore::Setup()
{
  YgString EditorVariablesPath = GetPath("/Configuration/Editor/editor_variables.yml");

  BuildApplicationCoreCompoments();
  InitAudioEngine();

  LauncherProjectPicker project = RequestLauncher();
  if (!project.UserWantToExit) {

    m_mode = Yeager::AppEditor;
    m_Window->RegenerateMainWindow(1920, 1080, RequestWindowEngineName(project), m_Input->MouseCallback);
    m_Interface->RequestRestartInterface(m_Window);
    m_Scene = new Yeager::Scene(project.m_Name, project.m_SceneType, project.m_SceneRenderer, this);
    m_Launcher->GetNewProjectLaoded() ? m_Scene->Save() : m_Scene->Load(project.m_ProjectPath);
    m_Scene->GetSerial().ReadEditorVariables(EditorVariablesPath.c_str());
    m_Scene->GetSerial().ReadSceneShadersConfig(GetPath("/Configuration/Editor/Shaders/default_shaders.yaml"));

  } else {
    TerminateAudioEngine();
  }
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_Window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  delete m_EditorExplorer;
  delete m_EditorCamera;
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
  Yeager::SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f), irrklang::vec3df(0.0f, 1.0f, 0.0f));
}

void ApplicationCore::Render()
{
  OpenGLFunc();

  Yeager::Skybox skybox("Skybox", ObjectGeometryType::ECube, this, false);
  skybox.BuildSkyboxFromImport(GetPath("/Assets/ImportedModels/Skybox/skybox.obj"));

  std::vector<YgVector3> pos;
  for (int x = 0; x < 20; x++) {
    for (int y = 0; y < 20; y++) {
      YgVector3 vec;
      vec.x = RandomFloatRange(0, 10) * x;
      vec.y = RandomFloatRange(0, 10);
      vec.z = RandomFloatRange(0, 10) * y;
      pos.push_back(vec);
    }
  }

  LightHandle light(ShaderFromVarName("SimpleAnimated"));
  LightSource sources(ShaderFromVarName("Simple"));
  ObjectPointLight obj;
  sources.AddSpotLight(obj, this);

  InstancedAnimatedObject swat("Swat", this, 400);
  swat.ImportObjectFromFile(GetPath("/Assets/ImportedModels/style/style.dae").c_str(), true);
  swat.BuildProp(pos, ShaderFromVarName("SimpleAnimated"));
  //swat.BuildProp(pos, ShaderFromVarName("SimpleAnimated"));
  Animation SwatCover(GetPath("/Assets/ImportedModels/style/style.dae"), &swat);
  AnimationEngine engine(&SwatCover);

  Object pacific("Pacific", this);
  pacific.ImportObjectFromFile(GetPath("/Assets/ImportedModels/f18/F-18.obj").c_str(), false);
  pacific.GetTransformationPtr()->scale = YgVector3(0.05f);
 
  
  while (ShouldRender()) {

    glfwPollEvents();
    OpenGLClear();

    m_Interface->InitRenderFrame();

    UpdateDeltaTime();
    UpdateWorldMatrices();
    UpdateListenerPosition();

    ManifestShaderProps(ShaderFromVarName("Skybox"));
    skybox.Draw(ShaderFromVarName("Skybox"), YgMatrix3(m_WorldMatrices.View), m_WorldMatrices.Projection);

    ygAudioEngine->update();
    engine.UpdateAnimation(m_DeltaTime);

    ManifestShaderProps(ShaderFromVarName("TerrainGeneration"));

    light.BuildShaderProps(ShaderFromVarName("SimpleAnimated"), GetCamera()->GetPosition(), GetCamera()->GetDirection(),
                           32.0f);

    ManifestShaderProps(ShaderFromVarName("SimpleAnimated"));
    auto transf = engine.GetFinalBoneMatrices();
    for (int x = 0; x < MAX_BONES; x++) {
      ShaderFromVarName("SimpleAnimated")->SetMat4("finalBonesMatrices[" + std::to_string(x) + "]", transf.at(x));
    }
    swat.Draw(ShaderFromVarName("SimpleAnimated"));

    ManifestShaderProps(ShaderFromVarName("Simple"));
    sources.BuildShaderProps(ShaderFromVarName("Simple"), GetCamera()->GetPosition(), GetCamera()->GetDirection(),
                             32.0f);
   
    pacific.Draw(ShaderFromVarName("Simple"));
    for (const auto& obj : *GetScene()->GetObjects()) {
      obj->Draw(ShaderFromVarName("Simple"));
    }

    ManifestShaderProps(ShaderFromVarName("Light"));
    sources.DrawLightSources(ShaderFromVarName("Light"));

    GetInterface()->RenderUI();
    m_Interface->TerminateRenderFrame();
    GetInput()->ProcessInputRender(GetWindow(), m_DeltaTime);
    glfwSwapBuffers(GetWindow()->getWindow());
  }

  GetScene()->Save();
  TerminateAudioEngine();
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
  glEnable(GL_CULL_FACE);
}

void ApplicationCore::OpenGLClear()
{
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
