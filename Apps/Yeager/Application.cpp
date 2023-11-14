#include "Application.h"
using namespace Yeager;

ApplicationCore::ApplicationCore()
{
  Log(INFO, "Application running");
}

void ApplicationCore::Setup(ApplicationCoreSetup setup)
{
  m_interface = setup.m_interface;
  m_input = setup.m_input;
  m_window = setup.m_window;
  m_explorer = setup.m_explorer;
  m_camera = setup.m_camera;
  m_scene = setup.m_scene;
}

bool ApplicationCore::ShouldRender()
{
  return (glfwWindowShouldClose(m_window->getWindow())) ? false : true;
}

ApplicationCore::~ApplicationCore()
{
  Log(INFO, "Application shutdown");
}

Interface* ApplicationCore::GetInterface()
{
  return m_interface.get();
}
Input* ApplicationCore::GetInput()
{
  return m_input.get();
}
Window* ApplicationCore::GetWindow()
{
  return m_window.get();
}
EditorExplorer* ApplicationCore::GetExplorer()
{
  return m_explorer.get();
}
EditorCamera* ApplicationCore::GetCamera()
{
  return m_camera.get();
}
Yeager::Scene* ApplicationCore::GetScene()
{
  return m_scene.get();
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

void ApplicationCore::ManifestShaderProps(Yeager::Shader* shader, YgMatrix4 view, YgMatrix4 projection,
                                          YgVector3 viewPos)
{
  shader->UseShader();
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);
  shader->SetVec3("viewPos", viewPos);
}

void ApplicationCore::CheckGLAD()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Yeager::Log(ERROR, "Cannot load GLAD!");
  }
}

void ApplicationCore::Render()
{
  Yeager::Log(INFO, "Rendering OpenGL");
  InitAudioEngine();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glCullFace(GL_FRONT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  std::vector<YgString> faces = {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};

  for (

      unsigned int x = 0; x < 6; x++) {
    YgString path = GetPath("/Assets/textures/skybox/") + faces[x];
    faces[x] = path;
  }
  GetInterface();
  GetScene()->GetSerial().ReadSceneShadersConfig(GetPath("/Configuration/Editor/Shaders/default_shaders.yaml"));
  Yeager::Texture2D wall_texture(GetPath("/Assets/textures/neco.jpg").c_str(), "Wall texture");
  Yeager::Texture2D bocchi_texture(GetPath("/Assets/textures/bocchi.jpg").c_str(), "Bocchi texture");
  Yeager::Skybox skybox(faces, "Skybox");

  GetScene()->GetSerial().DeserializeScene(GetScene(), GetPath("/Configuration/Scenes/DefaultScene.yaml"));

  while (ShouldRender()) {
    float time = (float)glfwGetTime();
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ygAudioEngine->update();

    YgMatrix4 projection =
        glm::perspective(glm::radians(45.0f), (float)ygWindowWidth / (float)ygWindowHeight, 0.1f, 1000.0f);
    YgMatrix4 view = GetCamera()->ReturnViewMatrix();
    YgVector3 viewPos = GetCamera()->GetPosition();

    irrklang::vec3df cameraPos = Yeager::YgVec3_to_Vec3df(viewPos);
    irrklang::vec3df cameraDir = Yeager::YgVec3_to_Vec3df(GetCamera()->GetDirection());
    Yeager::SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                            irrklang::vec3df(0.0f, 1.0f, 0.0f));

    for (unsigned int x = 0; x < GetScene()->GetAudios3D()->size(); x++) {
      YgVector3 pos = GetScene()->GetAudios3D()->at(x)->GetVector3Position();
    }

    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.Draw(Yeager::ShaderFromVarName("Skybox"), YgMatrix3(view), projection);

    ManifestShaderProps(Yeager::ShaderFromVarName("Common"), view, projection, viewPos);

    ManifestShaderProps(Yeager::ShaderFromVarName("Simple"), view, projection, viewPos);

    for (unsigned int x = 0; x < GetScene()->GetImportedObjects()->size(); x++) {
      GetScene()->GetImportedObjects()->at(x)->Draw(Yeager::ShaderFromVarName("Simple"));
    }

    ManifestShaderProps(Yeager::ShaderFromVarName("Geometry"), view, projection, viewPos);
    for (unsigned int x = 0; x < GetScene()->GetGeometry()->size(); x++) {
      GetScene()->GetGeometry()->at(x)->Draw(Yeager::ShaderFromVarName("Geometry"));
    }

    GetInterface()->RenderUI();
    GetInput()->ProcessInputRender(GetWindow(), deltaTime);
    glfwSwapBuffers(GetWindow()->getWindow());
  }
  GetScene()->Save();
  TerminateAudioEngine();
}
