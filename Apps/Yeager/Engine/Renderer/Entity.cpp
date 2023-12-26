#include "Entity.h"
using namespace Yeager;

Transformation Yeager::GetDefaultTransformation()
{
  Transformation trans;
  trans.model = YgMatrix4(1.0f);
  trans.position = YgVector3(0.0f);
  trans.rotation = YgVector3(0.0f);
  trans.scale = YgVector3(1.0f);
  return trans;
}

void Yeager::ProcessTransformation(Transformation* trans)
{
  trans->model = YgMatrix4(1.0f);
  trans->model = glm::translate(trans->model, trans->position);
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.x), YgVector3(1.0f, 0.0f, 0.0f));
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.y), YgVector3(0.0f, 1.0f, 0.0f));
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.z), YgVector3(0.0f, 0.0f, 1.0f));
  trans->model = glm::scale(trans->model, trans->scale);
}

unsigned int Entity::m_entityCountId = 0;

Entity::Entity(YgString name) : m_Name(name), m_id(m_entityCountId++)
{
  Yeager::Log(INFO, "Creating Entity name {}, ID {}", m_Name, m_id);
}

Entity::~Entity()
{
  Yeager::Log(INFO, "Destroying Entity name {}, ID {}", m_Name, m_id);
}

YgString Entity::GetName()
{
  return m_Name;
}
unsigned int Entity::GetId()
{
  return m_id;
}

GameEntity::GameEntity(YgString name) : Entity(name)
{
  Yeager::Log(INFO, "Creating GameEntity name {} ID {}", m_Name, m_id);
  m_EntityTransformation = GetDefaultTransformation();
}

GameEntity::~GameEntity()
{
  Yeager::Log(INFO, "Destrorying GameEntity name {} ID {}", m_Name, m_id);
}
Transformation GameEntity::GetTransformation()
{
  return m_EntityTransformation;
}

Transformation* GameEntity::GetTransformationPtr()
{
  return &m_EntityTransformation;
}

void GameEntity::ProcessTransformation(Yeager::Shader* shader)
{
  m_EntityTransformation.model = glm::translate(m_EntityTransformation.model, m_EntityTransformation.position);
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.x), YgVector3(1.0f, 0.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.y), YgVector3(0.0f, 1.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.z), YgVector3(0.0f, 0.0f, 1.0f));
  m_EntityTransformation.model = glm::scale(m_EntityTransformation.model, m_EntityTransformation.scale);

  shader->SetMat4("model", m_EntityTransformation.model);
  m_EntityTransformation.model = YgMatrix4(1.0f);
}

DrawableEntity::DrawableEntity(YgString name)
{
  Yeager::Log(INFO, "Creating Drawable Entity {}", name);

  glGenVertexArrays(1, &m_Vao);
  glGenBuffers(1, &m_Vbo);
  glGenBuffers(1, &m_Ebo);
}
DrawableEntity::~DrawableEntity()
{
  Terminate();
}

void DrawableEntity::Terminate()
{
  Yeager::Log(INFO, "Terminate drawable entity was been called [{}]", m_Name.c_str());
  glDeleteVertexArrays(1, &m_Vao);
  glDeleteBuffers(1, &m_Vbo);
  glDeleteBuffers(1, &m_Ebo);
}

void DrawableEntity::Draw(Shader* shader)
{
  YEAGER_NOT_IMPLEMENTED("Draw from drawable entity");
}
