#include "Entity.h"
#include "Editor/Utils/NodeHierarchy.h"
#include "Main/Core/Application.h"

using namespace Yeager;

Uint Entity::mEntityCount = 0;

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
  if (mApplication->GetScene() != YEAGER_NULLPTR) {
    mNode = std::make_shared<NodeComponent>(mApplication, this, parent);
    Yeager::LogDebug(INFO, "Node build for {}, UUID {}, parent {}", mName, uuids::to_string(mEntityUUID),
                     parent->IsRoot() ? "Root" : parent->GetEntity()->GetName());
  }
}

EditorEntity::EditorEntity(const EntityBuilder& builder) : Entity(builder)
{
  if (mApplication) {
    mToolbox = std::make_shared<ToolboxHandle>(this);
    mApplication->GetScene()->GetToolboxs()->push_back(mToolbox);
  }
}

EditorEntity::~EditorEntity()
{
  if (mToolbox) {
    mToolbox->SetScheduleDeletion(true);  // Notify the scene vector of the removal of the toolbox
    mToolbox.reset();
  }  // Kills the entity ownership on the toolbox
  mNode.reset();
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

Entity::Entity(const EntityBuilder& builder)
    : mName(builder.Name), mEntityUUID(builder.UUID), mApplication(builder.Application), mType(builder.Type)
{
  Yeager::LogDebug(INFO, "Creating Entity name {}, UUID {}", mName, uuids::to_string(mEntityUUID));
}

Entity::~Entity()
{
  Yeager::LogDebug(INFO, "Destroying Entity name {}, UUID {}", mName, uuids::to_string(mEntityUUID));
}

String Entity::GetName()
{
  return mName;
}
uuids::uuid Entity::GetEntityUUID()
{
  return mEntityUUID;
}

GameEntity::GameEntity(const EntityBuilder& builder)
    : EditorEntity(builder), mEntityTransformation(Transformation3D::GetDefault())
{
  Yeager::LogDebug(INFO, "Creating GameEntity name {} UUID {}", mName, uuids::to_string(mEntityUUID));
}

GameEntity::~GameEntity()
{
  Yeager::LogDebug(INFO, "Destroying GameEntity name {} UUID {}", mName, uuids::to_string(mEntityUUID));
}
Transformation3D GameEntity::GetTransformation()
{
  return mEntityTransformation;
}

Transformation3D* GameEntity::GetTransformationPtr()
{
  return &mEntityTransformation;
}

void GameEntity::ApplyTransformation(Yeager::Shader* shader)
{
  Transformation3D::Apply(&mEntityTransformation, shader);
}
