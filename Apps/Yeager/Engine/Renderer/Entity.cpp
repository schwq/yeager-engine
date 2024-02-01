#include "Entity.h"
#include "../../Application.h"
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

EditorEntity::EditorEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name)
    : Entity(type, app, name)
{}
EditorEntity::~EditorEntity() {}

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

Entity::Entity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name)
    : m_Name(name), m_id(m_entityCountId++), m_Application(app), m_Type(type)
{
  Yeager::Log(INFO, "Creating Entity name {}, ID {}", m_Name, m_id);
  m_Valid = true;
}

Entity::~Entity()
{
  Yeager::Log(INFO, "Destroying Entity name {}, ID {}", m_Name, m_id);
  m_Valid = false;
}

YgString Entity::GetName()
{
  return m_Name;
}
unsigned int Entity::GetId()
{
  return m_id;
}

GameEntity::GameEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, YgString name)
    : EditorEntity(type, app, name)
{
  Yeager::Log(INFO, "Creating GameEntity name {} ID {}", m_Name, m_id);
  m_EntityTransformation = GetDefaultTransformation();
  assert(app != YEAGER_NULLPTR);
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
