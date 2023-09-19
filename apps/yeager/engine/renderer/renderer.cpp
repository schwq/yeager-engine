#include "renderer.h"
#include "shapes.h"
#include "texture.h"

using namespace irrklang;

RendererEngine::RendererEngine(RendererEngineName name, Application* app)
    : m_app(app) {
  m_handle.name = name;
}

void RendererEngine::Render() {
  RendererOpenGL();
}

void RendererEngine::LaunchMusicEngineThread() {
  engine = createIrrKlangDevice();

  if (!engine) {
    YeagerLog(ERROR, kSystemLog, "Cannot create music engine!");
    return;
  } else {
    YeagerLog(INFO, kSystemLog, "Playing aot music bitch");
  }

  //engine->play2D(GetPath("/assets/sound/mac.wav").c_str(), true);
}

void RendererEngine::RendererOpenGL() {
  YeagerLog(INFO, kSystemLog, "Rendering OpenGL");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  std::vector<String> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                               "bottom.jpg", "front.jpg", "back.jpg"};

  for (uint x = 0; x < 6; x++) {
    String path = GetPath("/assets/textures/skybox/") + faces[x];
    faces[x] = path;
  }
  Shader skyboxShader(GetPath("/assets/shaders/skybox_fg.glsl").c_str(),
                      GetPath("/assets/shaders/skybox_vt.glsl").c_str(),
                      "Skybox Shader");
  Shader commonShader(GetPath("/assets/shaders/common_fg.glsl").c_str(),
                      GetPath("/assets/shaders/common_vt.glsl").c_str(),
                      "Common Shader");
  Shader imp_objectShader(GetPath("/assets/shaders/imp_object_fg.glsl").c_str(),
                          GetPath("/assets/shaders/imp_object_vt.glsl").c_str(),
                          "Imported Object Shader");
  Shader light_shader(GetPath("/assets/shaders/lighting_fg.glsl").c_str(),
                      GetPath("/assets/shaders/lighting_vt.glsl").c_str(),
                      "Lighting Shader");
  YeagerTexture2D wall_texture(GetPath("/assets/textures/neco.jpg").c_str()
                             , "Wall texture");
  YeagerTexture2D bocchi_texture(GetPath("/assets/textures/bocchi.jpg").c_str(),
                                "Bocchi texture");
  EngineSkybox skybox(faces, "Skybox");
  m_app->GetLighting()->AddLightSource("test light", &commonShader,
                                       &commonShader, m_app);
  std::thread musicEngine(&RendererEngine::LaunchMusicEngineThread, this);

  LightingManager light_manager;
  light_manager.AddLightSource("Light cube", &light_shader, &commonShader,
                               m_app);

  while (m_app->ShouldRendererActive()) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    Matrix4 projection = glm::perspective(
        glm::radians(45.0f), (float)kWindowX / (float)kWindowY, 0.1f, 1000.0f);

    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4 view = m_app->GetEditorCamera()->ReturnViewMatrix();
    skybox.Draw(&skyboxShader, Matrix3(view), projection);

    light_shader.UseShader();
    light_shader.SetMat4("view", view);
    light_shader.SetMat4("projection", projection);
    light_manager.DrawLights();

    commonShader.UseShader();
    commonShader.SetMat4("view", view);
    commonShader.SetMat4("projection", projection);
    commonShader.SetVec3("viewPos", m_app->GetEditorCamera()->GetPosition());
    for (uint cubes = 0; cubes < m_app->GetVectorUserCube()->size(); cubes++) {
      m_app->GetVectorUserCube()->at(cubes).Draw(&commonShader);
    }

    m_app->GetLighting()->DrawLights();

    imp_objectShader.UseShader();
    imp_objectShader.SetMat4("view", view);
    imp_objectShader.SetMat4("projection", projection);
    imp_objectShader.SetVec3("viewPos",
                             m_app->GetEditorCamera()->GetPosition());

    for (uint imported_objects = 0;
         imported_objects < m_user_imported_objects.size();
         imported_objects++) {
      m_user_imported_objects.at(imported_objects)->Draw(&imp_objectShader);
    }

    m_app->GetInterface()->RenderUI();
    m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager(), deltaTime);
    glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
  }
  engine->drop();
  glfwTerminate();
}

void ::RendererEngine::checkGLAD() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    YeagerLog(ERROR, kSystemLog, "Cannot load GLAD!");
  }
}