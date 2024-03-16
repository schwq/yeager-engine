#include "PhysicsHandle.h"
using namespace Yeager;
EntityPhysics::EntityPhysics(Yeager::GameEntity* entity, float weight, float gravity_const)
    : m_EntityPtr(entity), m_weight(weight), m_gravity_const(gravity_const)
{
  Yeager::Log(INFO, "Creating EntityPhysics name {} ID {}", m_EntityPtr->GetName(), m_EntityPtr->GetId());
}

EntityPhysics::~EntityPhysics()
{
  Yeager::Log(INFO, "Destrorying EntityPhsics, name {}, ID {}", m_EntityPtr->GetName(), m_EntityPtr->GetId());
}

void EntityPhysics::ApplyGravity()
{
  if (m_gravity_enabled) {
    Transformation* trans = m_EntityPtr->GetTransformationPtr();
    if (!m_reverse_gravity) {
      trans->position.y -= m_weight * m_gravity_const * m_delta_brake;
    } else {
      trans->position.y += m_weight * m_gravity_const * m_delta_brake;
    }
  }
}
