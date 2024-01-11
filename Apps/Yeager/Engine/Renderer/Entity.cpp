#include "Entity.h"
#include "../../Application.h"
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

<<<<<<< Updated upstream
GameEntity::GameEntity(yg_string name, Yeager::Texture2D* texture, Yeager::Shader* shader)
    : Entity(name), m_texture(texture), m_shader(shader)
=======
GameEntity::GameEntity(YgString name, ApplicationCore* app) : Entity(name), m_Application(app)
>>>>>>> Stashed changes
{
  Yeager::Log(INFO, kSystem, "Creating GameEntity name {} ID {}", m_name, m_id);
  m_transformation = GetDefaultTransformation();
}

GameEntity::~GameEntity()
{
  Yeager::Log(INFO, kSystem, "Destrorying GameEntity name {} ID {}", m_name, m_id);
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
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.x), yg_vec3(1.0f, 0.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.y), yg_vec3(0.0f, 1.0f, 0.0f));
  m_transformation.model =
      glm::rotate(m_transformation.model, glm::radians(m_transformation.rotation.z), yg_vec3(0.0f, 0.0f, 1.0f));
  m_transformation.model = glm::scale(m_transformation.model, m_transformation.scale);

  shader->SetMat4("model", m_transformation.model);
  m_transformation.model = yg_mat4(1.0f);
}

<<<<<<< Updated upstream
void GameEntity::SetPosition(yg_vec3 pos)
=======
void GameEntity::ProcessTransformationCollision(Shader* shader, AABBCollision* col) {
  m_EntityTransformation.model = glm::translate(m_EntityTransformation.model, m_EntityTransformation.position);
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.x), YgVector3(1.0f, 0.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.y), YgVector3(0.0f, 1.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.z), YgVector3(0.0f, 0.0f, 1.0f));
  m_EntityTransformation.model = glm::scale(m_EntityTransformation.model, m_EntityTransformation.scale);

  shader->SetMat4("model", m_EntityTransformation.model);
  
  col->Draw(m_Application->ShaderFromVarName("Collision"), m_EntityTransformation.model);  
  m_EntityTransformation.model = YgMatrix4(1.0f);
}

DrawableEntity::DrawableEntity(YgString name)
>>>>>>> Stashed changes
{
  m_transformation.position = pos;
}