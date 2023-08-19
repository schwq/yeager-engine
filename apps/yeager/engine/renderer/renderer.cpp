#include "renderer.h"
#include <GLFW/glfw3.h>
#include <filesystem>

RendererEngine::RendererEngine(RendererEngineName name, Application *app)
    : m_app(app)
{
  m_handle.name = name;
}

void RendererEngine::Render() { RendererOpenGL(); }

void RendererEngine::RendererOpenGL()
{
  VLOG_F(INFO, "Rendering OpenGL");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  std::vector<String> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                               "bottom.jpg", "front.jpg", "back.jpg"};

  for (uint x = 0; x < 6; x++) {
    String path =
        "C:\\Users\\schwq\\OneDrive\\Documentos\\GitHub\\yeager-"
        "engine\\apps\\yeager\\assets\\textures\\skybox\\" +
        faces[x];
    faces[x] = path;
  }
  Shader skyboxShader(
      "C:\\Users\\schwq\\OneDrive\\Documentos\\GitHub\\yeager-"
      "engine\\apps\\yeager\\assets\\shaders\\skybox_fg.glsl",
      "C:\\Users\\schwq\\OneDrive\\Documentos\\GitHub\\yeager-"
      "engine\\apps\\yeager\\assets\\shaders\\skybox_vt.glsl",
      "Skybox Shader");
  EngineSkybox skybox(faces);

  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)kWindowX / (float)kWindowY, 0.1f, 1000.0f);

  while (m_app->ShouldRendererActive()) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Matrix4 view = m_app->GetEditorCamera()->ReturnViewMatrix();
    skybox.Draw(&skyboxShader, Matrix3(view), projection);

    m_app->GetInterface()->RenderUI();
    m_app->GetInput()->ProcessInputRender(m_app->GetWindowManager(), deltaTime);
    glfwSwapBuffers(m_app->GetWindowManager()->getWindow());
  }
}

void ::RendererEngine::checkGLAD()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    VLOG_F(ERROR, "Cannot load GLAD!");
  }
}