#include "Render.h"
#include "../Editor/ToolboxObj.h"
#include "../Media/AudioHandle.h"
#include "Geometry.h"
#include "ImportedObj.h"
#include "TextureHandle.h"

using namespace irrklang;

RendererEngine::RendererEngine(RendererEngineName name, Application* app)
    : m_app(app)
{
  m_handle.name = name;
}

void RendererEngine::Render()
{
  RendererOpenGL();
}

void RendererEngine::ManifestShaderProps(Yeager::Shader* shader, yg_mat4 view,
                                         yg_mat4 projection, yg_vec3 viewPos)
{
  shader->UseShader();
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);
  shader->SetVec3("viewPos", viewPos);
}

void RendererEngine::RendererOpenGL()
{
  Yeager::Log(INFO, kSystem, "Rendering OpenGL");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  std::vector<yg_string> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                                  "bottom.jpg", "front.jpg", "back.jpg"};

  for (yg_uint x = 0; x < 6; x++) {
    yg_string path = GetPath("/Assets/textures/skybox/") + faces[x];
    faces[x] = path;
  }
  Yeager::Shader skyboxShader(GetPath("/Assets/shaders/skybox_fg.glsl").c_str(),
                              GetPath("/Assets/shaders/skybox_vt.glsl").c_str(),
                              "Skybox Yeager::Shader");
  Yeager::Shader commonShader(GetPath("/Assets/shaders/common_fg.glsl").c_str(),
                              GetPath("/Assets/shaders/common_vt.glsl").c_str(),
                              "Common Yeager::Shader");
  Yeager::Shader imp_objectShader(
      GetPath("/Assets/shaders/imp_object_fg.glsl").c_str(),
      GetPath("/Assets/shaders/imp_object_vt.glsl").c_str(),
      "Imported Object Shader");
  Yeager::Shader light_shader(
      GetPath("/Assets/shaders/lighting_fg.glsl").c_str(),
      GetPath("/Assets/shaders/lighting_vt.glsl").c_str(), "Lighting Shader");
  Yeager::Shader simple_shader(
      GetPath("/Assets/shaders/simple_fg.glsl").c_str(),
      GetPath("/Assets/shaders/simple_vt.glsl").c_str(), "Simple Shader");
  Yeager::Shader geometryShader(
      GetPath("/Assets/shaders/geometry_fg.glsl").c_str(),
      GetPath("/Assets/shaders/geometry_vt.glsl").c_str(), "Geometry Shader");
  Yeager::Texture2D wall_texture(GetPath("/Assets/textures/neco.jpg").c_str(),
                                 "Wall texture");
  Yeager::Texture2D bocchi_texture(
      GetPath("/Assets/textures/bocchi.jpg").c_str(), "Bocchi texture");
  Yeager::Skybox skybox(faces, "Skybox");
  ImportedObject obj1(GetPath("/Assets/imported_models/backpack/backpack.obj"),
                      m_app, "BackPack", true);
  ImportedObject obj2(
      GetPath("/Assets/imported_models/neco_arc/neco.obj").c_str(), m_app,
      "Neco arc", false);
  Yeager::Texture2D cube_tex(GetPath("/Assets/textures/wall.jpg").c_str(),
                             "wall texture");
  Yeager::Geometry cube1("Cube 1", yg_vec3(1.0f), Yeager::kCube, m_app, false,
                         &cube_tex);
  cube1.SetPosition(yg_vec3(2.0f, 0.0f, 0.0f));
  Yeager::AudioHandle audio1(GetPath("/Assets/sound/aot01.wav"), "AOT01", true);
  Yeager::ToolBoxObject toolbox;
  toolbox.SetType(ExplorerObjectType::kAudio);
  toolbox.SetAudio(&audio1);
  m_app->GetExplorer()->AddObject(toolbox);
  audio1.Play();
  while (m_app->ShouldRendererActive()) {
    float time = (float)glfwGetTime();
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    audio1.Update();
    yg_mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)kWindowX / (float)kWindowY, 0.1f, 1000.0f);
    yg_mat4 view = m_app->GetEditorCamera()->ReturnViewMatrix();
    yg_vec3 viewPos = m_app->GetEditorCamera()->GetPosition();

    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.Draw(&skyboxShader, yg_mat3(view), projection);

    ManifestShaderProps(&commonShader, view, projection, viewPos);

    ManifestShaderProps(&simple_shader, view, projection, viewPos);
    //obj1.Draw(&simple_shader);
    obj2.Draw(&simple_shader);

    ManifestShaderProps(&imp_objectShader, view, projection, viewPos);

    for (yg_uint imported_objects = 0;
         imported_objects < m_app->GetImportedObjects()->size();
         imported_objects++) {
      m_app->GetImportedObjects()->at(imported_objects).Draw(&imp_objectShader);
    }

    ManifestShaderProps(&geometryShader, view, projection, viewPos);
    cube1.ChangeColor(
        yg_vec3(1.0f * cos(time), 1.0f * sin(time), 1.0f * tan(time)));
    cube1.Draw(&geometryShader);

    m_app->GetInterface()->RenderUI();
    m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager(), deltaTime);
    glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
  }
  glfwTerminate();
}

void ::RendererEngine::checkGLAD()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Yeager::Log(ERROR, kSystem, "Cannot load GLAD!");
  }
}