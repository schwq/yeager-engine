#include "shapes.h"
#include <glm/ext/matrix_transform.hpp>

std::vector<GLfloat> GetCubeVertices() {
  return std::vector<GLfloat>{
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};
}

std::vector<uint> GetCubeIndices() {
  return std::vector<uint>{0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6,
                           5, 4, 6, 6, 4, 7, 4, 0, 7, 7, 0, 3,
                           3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
}

YeagerShape::YeagerShape(String name, Application* app,
                         YeagerTexture2D texture, EngineShapeType type)
    : m_name(name), m_app(app), m_texture(texture), m_type(type) {
  m_toolbox = new ToolBoxObject();
  m_toolbox->SetType(ExplorerObjectType::kShapes);
  m_toolbox->SetPosition(m_trans.PGetPosition());
  m_toolbox->SetRotation(m_trans.PGetRotation());
  m_toolbox->SetScale(m_trans.PGetScale());
  m_toolbox->SetTexture(&m_texture);
  m_app->GetExplorer()->AddObject(m_name.c_str(), ExplorerObjectType::kShapes,
                                  m_toolbox);
  Setup();
}

YeagerShape::YeagerShape(String name, Application* app, String texturePath,
                         EngineShapeType type)
    : m_name(name), m_app(app), m_type(type) {
  m_texture = YeagerTexture2D(texturePath.c_str(), "Yeager Shape");
  m_toolbox = new ToolBoxObject();
  m_toolbox->SetType(ExplorerObjectType::kShapes);
  m_toolbox->SetPosition(m_trans.PGetPosition());
  m_toolbox->SetRotation(m_trans.PGetRotation());
  m_toolbox->SetScale(m_trans.PGetScale());
  m_toolbox->SetTexture(&m_texture);
  m_app->GetExplorer()->AddObject(m_name.c_str(), ExplorerObjectType::kShapes,
                                  m_toolbox);
  Setup();
}

void YeagerShape::ProcessShapeType() {
  switch (m_type) {
    case EngineShapeType::kCube:
      m_vertices = GetCubeVertices();
      m_indices = GetCubeIndices();
      break;
    case EngineShapeType::kSphere:
    case EngineShapeType::kCone:
    default:
      YeagerLog(
          ERROR, kSystemLog,
          "Process shape type error, type havent been implemented!");
  }
}

void YeagerShape::Setup() {

  ProcessShapeType();

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat),
               &m_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint),
               &m_indices[0], GL_STATIC_DRAW);

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

void YeagerShape::ProcessTransformation(Shader* shader) {
  m_trans.SetModel(Matrix4(1.0f));
  m_trans.SetModel(glm::translate(m_trans.GetModel(), m_trans.GetPosition()));
  m_trans.SetModel(glm::rotate(m_trans.GetModel(),
                               glm::radians(m_trans.GetRotation().x),
                               Vector3(1.0f, 0.0f, 0.0f)));
  m_trans.SetModel(glm::rotate(m_trans.GetModel(),
                               glm::radians(m_trans.GetRotation().y),
                               Vector3(0.0f, 1.0f, 0.0f)));
  m_trans.SetModel(glm::rotate(m_trans.GetModel(),
                               glm::radians(m_trans.GetRotation().z),
                               Vector3(0.0f, 0.0f, 1.0f)));
  m_trans.SetModel(glm::scale(m_trans.GetModel(), m_trans.GetScale()));
  shader->SetMat4("model", m_trans.GetModel());
}

void YeagerShape::Draw(Shader* shader) {
  shader->UseShader();

  ProcessTransformation(shader);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture.GetID());

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<uint>(m_indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

YeagerShape::~YeagerShape() {
  delete m_toolbox;
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}
