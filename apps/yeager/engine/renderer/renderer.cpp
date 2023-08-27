#include "renderer.h"
#include "shapes.h"
#include "texture.h"

RendererEngine::RendererEngine(RendererEngineName name, Application* app)
    : m_app(app) {
  m_handle.name = name;
}

void RendererEngine::Render() {
  RendererOpenGL();
}

void RendererEngine::RendererOpenGL() {
  VLOG_F(INFO, "Rendering OpenGL");

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
                      "Skybox Shader", m_app);
  Shader commonShader(GetPath("/assets/shaders/common_fg.glsl").c_str(),
                      GetPath("/assets/shaders/common_vt.glsl").c_str(),
                      "Common Shader", m_app);
  Shader imp_objectShader(GetPath("/assets/shaders/imp_object_fg.glsl").c_str(),
                          GetPath("/assets/shaders/imp_object_vt.glsl").c_str(),
                          "Imported Object Shader", m_app);
  EngineTexture2D wall_texture(GetPath("/assets/textures/neco.jpg").c_str(),
                               m_app, "Wall texture");
  EngineTexture2D bocchi_texture(GetPath("/assets/textures/bocchi.jpg").c_str(),
                                 m_app, "Bocchi texture");
  YeagerCube cube("YaegerCube01", m_app, &wall_texture);
  YeagerCube bocchi("Bocchi", m_app, &bocchi_texture,
                    Vector3(2.0f, 2.0f, 2.0f));
  EngineSkybox skybox(faces, m_app);

  ImportedObject backpack(
      GetPath("/assets/imported_models/backpack/backpack.obj"), m_app,
      "Backpack");

  m_app->YeagerLog(INFO, ConsoleLogSender::kSystemLog, "FUCKKKKK {}", 0);

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

    commonShader.UseShader();
    commonShader.SetMat4("view", view);
    commonShader.SetMat4("projection", projection);
    cube.Draw(&commonShader);
    bocchi.Draw(&commonShader);
    for (uint cubes = 0; cubes < m_app->GetVectorUserCube()->size(); cubes++) {
      m_app->GetVectorUserCube()->at(cubes)->Draw(&commonShader);
    }

    imp_objectShader.UseShader();
    imp_objectShader.SetMat4("view", view);
    imp_objectShader.SetMat4("projection", projection);

    for (uint imported_objects = 0;
         imported_objects < m_app->GetImportedObjects()->size();
         imported_objects++) {
      m_app->GetImportedObjects()
          ->at(imported_objects)
          ->Draw(&imp_objectShader);
    }

    m_app->GetInterface()->RenderUI();
    m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager(), deltaTime);
    glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
  }

  glfwTerminate();
}

void ::RendererEngine::checkGLAD() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    VLOG_F(ERROR, "Cannot load GLAD!");
  }
}