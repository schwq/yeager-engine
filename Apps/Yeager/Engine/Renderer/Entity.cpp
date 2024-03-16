#include "Entity.h"
#include "../../Application.h"
#include "../Editor/NodeHierarchy.h"
using namespace Yeager;

unsigned int Entity::m_EntityCountID = 0;

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
      return "NULL";
  }
}

Transformation Yeager::GetDefaultTransformation()
{
  Transformation trans;
  trans.model = Matrix4(1.0f);
  trans.position = Vector3(0.0f);
  trans.rotation = Vector3(0.0f);
  trans.scale = Vector3(1.0f);
  return trans;
}

void EditorEntity::BuildNode(Yeager::NodeComponent* parent)
{
  if (m_Application->GetScene() != YEAGER_NULLPTR) {
    m_Node = new NodeComponent(m_Application, this, parent);
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
  YEAGER_DELETE(m_Node);
}

void Yeager::ApplyTransformation(Transformation* trans)
{
  trans->model = Matrix4(1.0f);
  trans->model = glm::translate(trans->model, trans->position);
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  trans->model = glm::rotate(trans->model, glm::radians(trans->rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  trans->model = glm::scale(trans->model, trans->scale);
}

Entity::Entity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name)
    : m_Name(name), m_EntityID(m_EntityCountID++), m_Application(app), m_Type(type)
{
  Yeager::Log(INFO, "Creating Entity name {}, ID {}", m_Name, m_EntityID);
}

Entity::~Entity()
{
  Yeager::Log(INFO, "Destroying Entity name {}, ID {}", m_Name, m_EntityID);
}

String Entity::GetName()
{
  return m_Name;
}
unsigned int Entity::GetId()
{
  return m_EntityID;
}

GameEntity::GameEntity(EntityObjectType::Enum type, Yeager::ApplicationCore* app, String name)
    : EditorEntity(type, app, name)
{
  Yeager::Log(INFO, "Creating GameEntity name {} ID {}", m_Name, m_EntityID);
  m_EntityTransformation = GetDefaultTransformation();
  assert(app != YEAGER_NULLPTR);
}

GameEntity::~GameEntity()
{
  Yeager::Log(INFO, "Destrorying GameEntity name {} ID {}", m_Name, m_EntityID);
}
Transformation GameEntity::GetTransformation()
{
  return m_EntityTransformation;
}

Transformation* GameEntity::GetTransformationPtr()
{
  return &m_EntityTransformation;
}

void GameEntity::ApplyTransformation(Yeager::Shader* shader)
{
  m_EntityTransformation.model = glm::translate(m_EntityTransformation.model, m_EntityTransformation.position);
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.x), Vector3(1.0f, 0.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.y), Vector3(0.0f, 1.0f, 0.0f));
  m_EntityTransformation.model = glm::rotate(
      m_EntityTransformation.model, glm::radians(m_EntityTransformation.rotation.z), Vector3(0.0f, 0.0f, 1.0f));
  m_EntityTransformation.model = glm::scale(m_EntityTransformation.model, m_EntityTransformation.scale);

  shader->SetMat4("model", m_EntityTransformation.model);
  m_EntityTransformation.model = Matrix4(1.0f);
}
