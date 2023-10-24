#include "Entity.h"
using namespace Yeager;

constexpr Transformation Yeager::GetDefaultTransformation()
{
  Transformation trans;
  trans.model = yg_mat4(1.0f);
  trans.position = yg_vec3(0.0f);
  trans.rotation = yg_vec3(0.0f);
  trans.scale = yg_vec3(1.0f);
  return trans;
}

yg_uint Entity::m_entityCountId = 0;

Entity::Entity(yg_string name) : m_name(name), m_id(m_entityCountId++)
{
  Yeager::Log(INFO, kSystem, "Creating Entity name {}, ID {}", m_name, m_id);
}

Entity::~Entity()
{
  Yeager::Log(INFO, kSystem, "Destroying Entity name {}, ID {}", m_name, m_id);
}

yg_string Entity::GetName()
{
  return m_name;
}
yg_uint Entity::GetId()
{
  return m_id;
}

GameEntity::GameEntity(yg_string name, Yeager::Texture2D* texture,
                       Yeager::Shader* shader)
    : Entity(name), m_texture(texture), m_shader(shader)
{
  m_transformation = GetDefaultTransformation();
}

GameEntity::~GameEntity() {}

constexpr Yeager::Texture2D* GameEntity::GetTexture()
{
  return m_texture;
}
constexpr Yeager::Shader* GameEntity::GetShader()
{
  return m_shader;
}
constexpr Transformation GameEntity::GetTransformation()
{
  return m_transformation;
}

Transformation* GameEntity::GetTransformationPtr()
{
  return &m_transformation;
}

void GameEntity::ProcessTransformation(Shader* shader)
{
  m_transformation.model =
      glm::translate(m_transformation.model, m_transformation.position);
  m_transformation.model = glm::rotate(
      m_transformation.model, glm::radians(m_transformation.rotation.x),
      yg_vec3(1.0f, 0.0f, 0.0f));
  m_transformation.model = glm::rotate(
      m_transformation.model, glm::radians(m_transformation.rotation.y),
      yg_vec3(0.0f, 1.0f, 0.0f));
  m_transformation.model = glm::rotate(
      m_transformation.model, glm::radians(m_transformation.rotation.z),
      yg_vec3(0.0f, 0.0f, 1.0f));
  m_transformation.model =
      glm::scale(m_transformation.model, m_transformation.scale);

  shader->SetMat4("model", m_transformation.model);
  m_transformation.model = yg_mat4(1.0f);
}

void GameEntity::SetPosition(yg_vec3 pos)
{
  m_transformation.position = pos;
}