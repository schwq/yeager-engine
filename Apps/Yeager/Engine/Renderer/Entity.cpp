#include "Entity.h"
#include "../../Application.h"
#include "../Editor/NodeHierarchy.h"

using namespace Yeager;

Uint Entity::m_EntityCountID = 0;

String EntityObjectType::ToString(EntityObjectType::Enum type)
{
  switch (type) {
    case OBJECT:
      return "Object";
    case OBJECT_ANIMATED:
      return "Animated Object";
    case OBJECT_INSTANCED:
      return "Instanced Object";
    case OBJECT_INSTANCED_ANIMATED:
      return "Animated Instanced Object";
    case AUDIO_HANDLE:
      return "Audio";
    case AUDIO_3D_HANDLE:
      return "3D Audio";
    case TEXTURE:
      return "Texture";
    case LIGHT_HANDLE:
      return "Lighting";
    case SKYBOX:
      return "Skybox";
    case CAMERA:
      return "Camera";
    case UNDEFINED:
    default:
      return "Undefined";
  }
}

void EditorEntity::BuildNode(std::shared_ptr<NodeComponent> parent)
{
  if (m_Application->GetScene() != YEAGER_NULLPTR) {
    m_Node = std::make_shared<NodeComponent>(m_Application, this, parent);
    Yeager::LogDebug(INFO, "Node build for {}, ID {}, parent {}", m_Name, m_EntityID,
                     parent->IsRoot() ? "Root" : parent->GetEntity()->GetName());
  }
}

EditorEntity::EditorEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name)
    : Entity(type, app, name)
{
  if (m_Application) {
    m_Toolbox = std::make_shared<ToolboxHandle>(this);
    m_Application->GetScene()->GetToolboxs()->push_back(m_Toolbox);
  }
}

EditorEntity::~EditorEntity()
{
  if (m_Toolbox) {
    m_Toolbox->SetScheduleDeletion(true);  // Notify the scene vector of the removal of the toolbox
    m_Toolbox.reset();
  }  // Kills the entity ownership on the toolbox
  m_Node.reset();
}

void Transformation2D::Apply(Transformation2D* transformation)
{
  Transformation2D* t = transformation;
  t->model = glm::translate(t->model, Vector3(t->position.x, t->position.y, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  t->model = glm::scale(t->model, Vector3(t->scale.x, t->scale.y, 0.0f));
}

void Transformation2D::Apply(Transformation2D* transformation, Yeager::Shader* shader)
{
  Transformation2D* t = transformation;
  t->model = glm::translate(t->model, Vector3(t->position.x, t->position.y, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  t->model = glm::scale(t->model, Vector3(t->scale.x, t->scale.y, 0.0f));
  shader->SetMat4("model", t->model);
  t->model = YEAGER_IDENTITY_MATRIX4x4;
}

Matrix4 Transformation2D::Apply(const Transformation2D& transformation)
{
  Matrix4 model = YEAGER_IDENTITY_MATRIX4x4;
  model = glm::translate(model, Vector3(transformation.position.x, transformation.position.y, 0.0f));
  model = glm::rotate(model, glm::radians(transformation.rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(transformation.rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, Vector3(transformation.scale.x, transformation.scale.y, 0.0f));
  return model;
}

Transformation2D Transformation2D::GetDefault()
{
  return Transformation2D(YEAGER_ZERO_VECTOR2, YEAGER_ZERO_VECTOR2, Vector2(1.0f));
}

void Transformation3D::Apply(Transformation3D* transformation)
{
  Transformation3D* t = transformation;
  t->model = glm::translate(t->model, t->position);
  t->model = glm::rotate(t->model, glm::radians(t->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  t->model = glm::scale(t->model, t->scale);
}

void Transformation3D::Apply(Transformation3D* transformation, Yeager::Shader* shader)
{
  Transformation3D* t = transformation;
  t->model = glm::translate(t->model, t->position);
  t->model = glm::rotate(t->model, glm::radians(t->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  t->model = glm::rotate(t->model, glm::radians(t->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  t->model = glm::scale(t->model, t->scale);
  shader->SetMat4("model", t->model);
  t->model = YEAGER_IDENTITY_MATRIX4x4;
}

Matrix4 Transformation3D::Apply(const Transformation3D& transformation)
{
  Matrix4 model = YEAGER_IDENTITY_MATRIX4x4;
  model = glm::translate(model, transformation.position);
  model = glm::rotate(model, glm::radians(transformation.rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(transformation.rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(transformation.rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, transformation.scale);
  return model;
}

Transformation3D Transformation3D::GetDefault()
{
  return Transformation3D(YEAGER_ZERO_VECTOR3, YEAGER_ZERO_VECTOR3, Vector3(1.0f));
}

Entity::Entity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name)
    : m_Name(name), m_EntityID(m_EntityCountID++), m_Application(app), m_Type(type)
{
  Yeager::LogDebug(INFO, "Creating Entity name {}, ID {}", m_Name, m_EntityID);
}

Entity::~Entity()
{
  Yeager::LogDebug(INFO, "Destroying Entity name {}, ID {}", m_Name, m_EntityID);
}

String Entity::GetName()
{
  return m_Name;
}
Uint Entity::GetEntityID()
{
  return m_EntityID;
}

GameEntity::GameEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name)
    : EditorEntity(type, app, name), m_EntityTransformation(Transformation3D::GetDefault())
{
  Yeager::LogDebug(INFO, "Creating GameEntity name {} ID {}", m_Name, m_EntityID);
}

GameEntity::~GameEntity()
{
  Yeager::LogDebug(INFO, "Destroying GameEntity name {} ID {}", m_Name, m_EntityID);
}
Transformation3D GameEntity::GetTransformation()
{
  return m_EntityTransformation;
}

Transformation3D* GameEntity::GetTransformationPtr()
{
  return &m_EntityTransformation;
}

void GameEntity::ApplyTransformation(Yeager::Shader* shader)
{
  Transformation3D::Apply(&m_EntityTransformation, shader);
}
