#include "Render.h"
#include "../../Scene.h"
#include "../../Serialization.h"
#include "../Editor/ToolboxObj.h"
#include "../Media/AudioHandle.h"
#include "Geometry.h"
#include "ImportedObj.h"
#include "TextureHandle.h"

using namespace irrklang;

RendererEngine::~RendererEngine()
{
  Yeager::Log(INFO, kSystem, "Destrorying RendererEngine!");
};

RendererEngine::RendererEngine(RendererEngineName name, Application* app) : m_app(app)
{
  m_handle.name = name;
}

void RendererEngine::Render()
{
  RendererOpenGL();
}

void RendererEngine::ManifestShaderProps(Yeager::Shader* shader, yg_mat4 view, yg_mat4 projection, yg_vec3 viewPos)
{
  shader->UseShader();
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);
  shader->SetVec3("viewPos", viewPos);
}

void RendererEngine::RendererOpenGL()
{
  Yeager::Log(INFO, kSystem, "Rendering OpenGL");
  InitAudioEngine();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  std::vector<yg_string> faces = {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};

  for (yg_uint x = 0; x < 6; x++) {
    yg_string path = GetPath("/Assets/textures/skybox/") + faces[x];
    faces[x] = path;
  }
  Yeager::Scene scene("Default Scene", Yeager::Scene3D, Yeager::OpenGL3_3);
  scene.LoadEditorColorscheme(m_app->GetInterface());
  Yeager::Texture2D wall_texture(GetPath("/Assets/textures/neco.jpg").c_str(), "Wall texture");
  Yeager::Texture2D bocchi_texture(GetPath("/Assets/textures/bocchi.jpg").c_str(), "Bocchi texture");
  Yeager::Skybox skybox(faces, "Skybox");

  while (m_app->ShouldRendererActive()) {
    float time = (float)glfwGetTime();
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ygAudioEngine->update();

    yg_mat4 projection = glm::perspective(glm::radians(45.0f), (float)kWindowX / (float)kWindowY, 0.1f, 1000.0f);
    yg_mat4 view = m_app->GetEditorCamera()->ReturnViewMatrix();
    yg_vec3 viewPos = m_app->GetEditorCamera()->GetPosition();

    irrklang::vec3df cameraPos = Yeager::YgVec3_to_Vec3df(viewPos);
    irrklang::vec3df cameraDir = Yeager::YgVec3_to_Vec3df(m_app->GetEditorCamera()->GetDirection());
    Yeager::SetListernerPos(cameraPos, cameraDir, irrklang::vec3df(0.0f, 0.0f, 0.0f),
                            irrklang::vec3df(0.0f, 1.0f, 0.0f));

    for (yg_uint x = 0; x < ygAudio3DHandles.size(); x++) {
      yg_vec3 pos = ygAudio3DHandles[x]->GetVector3Position();
    }

    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.Draw(Yeager::ShaderFromVarName("Skybox"), yg_mat3(view), projection);

    ManifestShaderProps(Yeager::ShaderFromVarName("Common"), view, projection, viewPos);

    ManifestShaderProps(Yeager::ShaderFromVarName("Simple"), view, projection, viewPos);

    for (yg_uint x = 0; x < ygImportedObjects.size(); x++) {
      ygImportedObjects[x]->Draw(Yeager::ShaderFromVarName("Simple"));
    }

    ManifestShaderProps(Yeager::ShaderFromVarName("Geometry"), view, projection, viewPos);
    for (yg_uint x = 0; x < yg_Shapes.size(); x++) {
      yg_Shapes[x]->Draw(Yeager::ShaderFromVarName("Geometry"));
    }

    m_app->GetInterface()->RenderUI();
    m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager(), deltaTime);
    glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
  }
  scene.Save();
  TerminateAudioEngine();
}

void ::RendererEngine::checkGLAD()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Yeager::Log(ERROR, kSystem, "Cannot load GLAD!");
  }
}