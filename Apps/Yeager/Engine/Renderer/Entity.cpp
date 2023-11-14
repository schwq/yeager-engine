#include "Entity.h"
using namespace Yeager;

constexpr Transformation Yeager::GetDefaultTransformation()
{
  Transformation trans;
  trans.model = YgMatrix4(1.0f);
  trans.position = YgVector3(0.0f);
  trans.rotation = YgVector3(0.0f);
  trans.scale = YgVector3(1.0f);
  return trans;
}

unsigned int Entity::m_entityCountId = 0;

Entity::Entity(YgString name) : m_name(name), m_id(m_entityCountId++)
{
  Yeager::Log(INFO, "Creating Entity name {}, ID {}", m_name, m_id);
}

Entity::~Entity()
{
  Yeager::Log(INFO, "Destroying Entity name {}, ID {}", m_name, m_id);
}

YgString Entity::GetName()
{
  return m_name;
}
unsigned int Entity::GetId()
{
  return m_id;
}

GameEntity::GameEntity(YgString name, Yeager::Texture2D* texture, Yeager::Shader* shader)
    : Entity(name), m_texture(texture), m_shader(shader)
{
  Yeager::Log(INFO, "Creating GameEntity name {} ID {}", m_name, m_id);
  m_transformation = GetDefaultTransformation();
}

GameEntity::~GameEntity()
{
  Yeager::Log(INFO, "Destrorying GameEntity name {} ID {}", m_name, m_id);
}

constexpr Yeager::Texture2D* GameEntity::GetTexture()
{
  return m_texture;
}
constexpr Yeager::Shader* GameEntity::GetShader()
{
  return m_shader;
}
Transformation GameEntity::GetTransformation()
{
  return m_transformation;
}

Transformation* GameEntity::GetTransformationPtr()
{
  return &m_transformation;
}

void GameEntity::ProcessTransformation(Shader* shader)
{
  m_transformation.model = glm::translate(m_transformation.model, m_transformation.position);
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.x), YgVector3(1.0f, 0.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.y), YgVector3(0.0f, 1.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.z), YgVector3(0.0f, 0.0f, 1.0f));
  m_transformation.model = glm::scale(m_transformation.model, m_transformation.scale);

  shader->SetMat4("model", m_transformation.model);
  m_transformation.model = YgMatrix4(1.0f);
}

void GameEntity::SetPosition(YgVector3 pos)
{
  m_transformation.position = pos;
}