#include "PlayableObject.h"
#include "../../../Application.h"
using namespace Yeager;

PlayableObject::PlayableObject(String name, ApplicationCore* application) : Object(name, application)
{
  SetEntityType(EntityObjectType::OBJECT_PLAYABLE);
  Yeager::LogDebug(INFO, "Created PLayable Object {} ID {}", name, m_EntityID);
}

void PlayableObject::Draw(Yeager::Shader* shader, float delta)
{
  ProcessPlayableSetOfRules();

  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && m_Render) {

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

PlayableAnimatedObject::PlayableAnimatedObject(String name, ApplicationCore* application)
    : AnimatedObject(name, application)
{
  SetEntityType(EntityObjectType::OBJECT_ANIMATED_PLAYABLE);
  Yeager::LogDebug(INFO, "Created Animated Playable Object {} ID {}", name, m_EntityID);
}

void PlayableAnimatedObject::Draw(Yeager::Shader* shader, float delta)
{
  ProcessPlayableSetOfRules();

  ProcessOnScreenProprieties();

  if (m_ObjectDataLoaded && m_Render) {
    shader->UseShader();
    if (m_InstancedType == ObjectInstancedType::eNON_INSTACED)
      ApplyTransformation(shader);
    DrawMeshes(shader);
  }

  PosProcessOnScreenProprieties();
}

void PlayableObject::ProcessPlayableSetOfRules()
{
  Yeager::BaseCamera* camera = m_Application->GetCamera();

  if (m_SetOfRules.FollowPlayerPosition) {
    m_EntityTransformation.position = camera->GetPosition() + m_SetOfRules.PositionOffset;
  }
}

void PlayableAnimatedObject::ProcessPlayableSetOfRules()
{
  Yeager::BaseCamera* camera = m_Application->GetCamera();

  if (m_SetOfRules.FollowPlayerPosition) {
    m_EntityTransformation.position = camera->GetPosition() + m_SetOfRules.PositionOffset;
  }
}