#include "PlayableObject.h"
#include "Main/Core/Application.h"

using namespace Yeager;

PlayableObject::PlayableObject(const EntityBuilder& builder) : Object(builder)
{
  SetEntityType(EntityObjectType::OBJECT_PLAYABLE);
  Yeager::LogDebug(INFO, "Created PLayable Object {} UUID {}", mName, uuids::to_string(mEntityUUID));
}

void PlayableObject::Draw(Yeager::Shader* shader, float delta)
{
  ProcessPlayableSetOfRules();

  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && bRender) {

    shader->UseShader();
    if (m_PhysicsType == ObjectPhysicsType::eDYNAMIC_BODY)
      m_Actor->ProcessTransformation(delta);
    ApplyTransformation(shader);

    if (m_GeometryType == ObjectGeometryType::eCUSTOM) {
      DrawModel(shader);
    } else {
      if (m_InstancedType == ObjectInstancedType::eNON_INSTACED) {
        DrawGeometry(shader);
      } else {
        DrawInstancedGeometry(shader);
      }
    }
  }

  PosProcessOnScreenProprieties();
}

PlayableAnimatedObject::PlayableAnimatedObject(const EntityBuilder& builder) : AnimatedObject(builder)
{
  SetEntityType(EntityObjectType::OBJECT_ANIMATED_PLAYABLE);
  Yeager::LogDebug(INFO, "Created Animated Playable Object {} UUID {}", mName, uuids::to_string(mEntityUUID));
}

void PlayableAnimatedObject::Draw(Yeager::Shader* shader, float delta)
{
  ProcessPlayableSetOfRules();

  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && bRender) {
    shader->UseShader();
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED)
      ApplyTransformation(shader);
    DrawMeshes(shader);
  }

  PosProcessOnScreenProprieties();
}

void PlayableObject::ProcessPlayableSetOfRules()
{
  Yeager::BaseCamera* camera = mApplication->GetCamera();

  if (m_SetOfRules.FollowPlayerPosition) {
    mEntityTransformation.position = camera->GetPosition() + m_SetOfRules.PositionOffset;
  }
}

void PlayableAnimatedObject::ProcessPlayableSetOfRules()
{
  Yeager::BaseCamera* camera = mApplication->GetCamera();

  if (m_SetOfRules.FollowPlayerPosition) {
    mEntityTransformation.position = camera->GetPosition() + m_SetOfRules.PositionOffset;
  }
}