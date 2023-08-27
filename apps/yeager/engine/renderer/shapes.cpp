#include "shapes.h"
#include <glm/ext/matrix_transform.hpp>

YeagerCube::~YeagerCube() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void YeagerCube::Setup() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

YeagerCube::YeagerCube(String name, Application* app, EngineTexture2D* texture,
                       const Vector3 position)
    : m_name(name), m_texture(texture), m_position(position), m_app(app) {

  Setup();

  m_toolbox = new ToolBoxObject();
  m_toolbox->SetType(ExplorerObjectType::kShapes);
  m_toolbox->SetPosition(&m_position);
  m_toolbox->SetRotation(&m_rotation);
  m_toolbox->SetScale(&m_scale);
  m_app->GetExplorer()->AddObject(m_name.c_str(), ExplorerObjectType::kShapes,
                                  m_toolbox);
}

void YeagerCube::ProcessTransformation(Shader* shader) {
  Matrix4 model = Matrix4(1.0f);
  model = glm::translate(model, m_position);
  model =
      glm::rotate(model, glm::radians(m_rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(m_rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(m_rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, m_scale);
  shader->SetMat4("model", model);
}

void YeagerCube::Draw(Shader* shader) {
  shader->UseShader();

  ProcessTransformation(shader);

  glBindVertexArray(m_vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture->GetID());

  glDrawArrays(GL_TRIANGLES, 0, 36);
}