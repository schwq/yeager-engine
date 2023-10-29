#include "Geometry.h"
using namespace Yeager;
std::vector<std::shared_ptr<Geometry>> yg_Shapes;

yg_string ShapeToString(GeometryShape shape)
{
  switch (shape) {
    case kCube:
      return "Cube";
    case kTriangle:
      return "Triangule";
    case kSphere:
      return "Sphere";
    default:
      return "Error_Invalid_Shape";
  }
}

std::vector<GLfloat> Geometry::GenerateCubeVertices()
{
  return std::vector<GLfloat>{
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};
}
std::vector<GLuint> Geometry::GenerateCubeIndices()
{
  return std::vector<GLuint>{0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                             4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
}

Geometry::Geometry(yg_string name, yg_vec3 color, GeometryShape shape, Application* app, bool is_color,
                   Yeager::Texture2D* texture)
    : m_color(color),
      m_shape(shape),
      m_app(app),
      GameEntity(name),
      m_physics(Yeager::EntityPhysics(this)),
      m_is_color(is_color),
      m_texture(texture)
{
  auto toolbox = std::make_shared<Yeager::ToolBoxObject>();
  toolbox->SetType(ExplorerObjectType::kShapes);
  toolbox->SetTransformation(this);
  toolbox->SetPhysics(&m_physics);
  m_toolboxs.push_back(toolbox);
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);
  m_transformation = GetDefaultTransformation();
  GenerateVerticesIndices();
  Setup();
}

Geometry::~Geometry()
{
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  Yeager::Log(INFO, kSystem, "Destrorying geometry shape name: {} ID: {}", m_name, m_id);
}

void Geometry::Setup()
{
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &m_vertices[0], GL_STATIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint),
  //             &m_indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Geometry::Draw(Shader* shader)
{
  glBindVertexArray(m_vao);
  m_physics.ApplyGravity();
  ProcessTransformation(shader);
  shader->UseShader();
  if (m_is_color) {
    shader->SetInt("material.is_solid_color", 0);
    shader->SetVec3("material.m_color", m_color);
  } else {
    shader->SetInt("material.is_solid_color", 1);
    shader->SetVec3("material.m_color", m_color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->GetID());
  }
  //glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void Geometry::GenerateVerticesIndices()
{
  switch (m_shape) {
    case kCube:
      m_vertices = GenerateCubeVertices();
      m_indices = GenerateCubeIndices();
      break;
    default:
      Yeager::Log(ERROR, kSystem, "Invalid geometry shape! Name {} Id {}", m_name, m_id);
  }
}