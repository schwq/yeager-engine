#include "PhysXActor.h"
#include "../../Application.h"
using namespace Yeager;
using namespace physx;

PhysXActor::PhysXActor(Yeager::ApplicationCore* application, Yeager::Object* object)
    : m_Application(application), m_Object(object)
{
  Yeager::LogDebug(INFO, "Created physx actor for object: {} ID {}", object->GetName(), object->GetId());
}

PhysXActor::~PhysXActor()
{
  Yeager::LogDebug(INFO, "Destroryed physx actor for object {} ID {}", m_Object->GetName(), m_Object->GetId());
}

void PhysXActor::BuildActor(const ObjectPhysXCreationBase& creation)
{
  Yeager::PhysXGeometryHandle* physics = m_Application->GetPhysXHandle()->GetGeometryHandle();
  const physx::PxU32 rigid = creation.Type == ObjectPhysicsType::eDYNAMIC_BODY ? YEAGER_PHYSX_RIGID_DYNAMIC_BODY
                                                                               : YEAGER_PHYSX_RIGID_STATIC_BODY;
  const Vector3 vec =
      m_Application->GetCamera()->GetPosition() + glm::normalize(m_Application->GetCamera()->GetDirection());
  ApplyTransformation(m_Object->GetTransformationPtr());

  if (creation.Type == ObjectPhysicsType::eUNDEFINED)
    return;  // No physics linked to the object

  m_Material = m_Application->GetPhysXHandle()->GetPxPhysics()->createMaterial(PX_MAX_F32, PX_MAX_F32, 0.5f);

  switch (m_Object->GetGeometry()) {
    case ObjectGeometryType::eCUBE: {
      m_Actor = physics->CreatePrimitiveBox(rigid, *m_Material, physx::PxTransform(creation.Position),
                                            physx::PxVec3(creation.Scale));
    } break;
    case ObjectGeometryType::eSPHERE: {
      m_Actor =
          physics->CreatePrimitiveSphere(rigid, *m_Material, physx::PxTransform(creation.Position), physx::PxU32(1.0f));
    } break;
    default:
      return;
  }

  if (rigid == YEAGER_PHYSX_RIGID_DYNAMIC_BODY) {
    physx::PxRigidBody* actor = static_cast<physx::PxRigidBody*>(m_Actor);

    const ObjectPhysXCreationDynamic& dynamic = static_cast<const ObjectPhysXCreationDynamic&>(creation);
    actor->setMass(dynamic.Mass);
  }
}

void PhysXActor::ProcessTransformation(float delta)
{
  const Yeager::ObjectPhysicsType::Enum type = m_Object->GetObjectPhysicsType();

  if (type == Yeager::ObjectPhysicsType::eDYNAMIC_BODY) {
    physx::PxRigidBody* actor = static_cast<physx::PxRigidBody*>(m_Actor);
    const Vector3 pos = PxVec3ToVector3(actor->getGlobalPose().p);
    const Vector3 rotv = PxVec3ToVector3(actor->getAngularVelocity());
    m_Object->GetTransformationPtr()->rotation += rotv;
    m_Object->GetTransformationPtr()->position = pos;
  }
}
