#include "PhysXCharacterController.h"
#include "PhysXHandle.h"

using namespace Yeager;
using namespace physx;

PhysXCharacterController::PhysXCharacterController(PhysXHandle* handle) : m_PhysXHandle(handle)
{
  Yeager::Log(INFO, "Created PhysX Character Controller!");
  Create();
}

PhysXCharacterController::~PhysXCharacterController()
{
  Yeager::Log(INFO, "Delete PhysX Character Controller");
  Delete();
}

void PhysXCharacterController::Create()
{
  m_ControllerManager = PxCreateControllerManager(*m_PhysXHandle->GetPxScene());
  m_ControllerManager->setOverlapRecoveryModule(true);
  /* Slower but more precise overlap testing */
  m_ControllerManager->setPreciseSweeps(true);
  m_ObstacleContext = m_ControllerManager->createObstacleContext();
}

physx::PxController* PhysXCharacterController::CreateController(const physx::PxControllerShapeType::Enum type,
                                                                const physx::PxF32 stepOffSet,
                                                                physx::PxF32 capsuleRadius, physx ::PxF32 capsuleHeight)
{
  PxController* controller = YEAGER_NULLPTR;
  switch (type) {
    case PxControllerShapeType::eBOX: {
      PxBoxControllerDesc desc;
      desc.stepOffset = stepOffSet;
      desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
      desc.slopeLimit = cosf(PxDegToRad(45.0f));
      desc.halfHeight = capsuleHeight;
      desc.reportCallback = this;
      if (desc.isValid()) {
        controller = m_ControllerManager->createController(desc);
      } else {
        Yeager::Log(ERROR, "PhysX Create Controller, desc is invalid!");
      }
    } break;
    case PxControllerShapeType::eCAPSULE: {
      PxCapsuleControllerDesc desc;
      desc.radius = capsuleRadius;
      desc.height = capsuleHeight;
      desc.stepOffset = stepOffSet;
      desc.slopeLimit = cosf(PxDegToRad(45.0f));
      desc.position = physx::PxExtendedVec3(0.0f, 20.0f, 0.0f);
      desc.material = m_PhysXHandle->GetPxPhysics()->createMaterial(1.0, 1.0f, 1.0f);
      desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
      desc.reportCallback = this;
      if (desc.isValid()) {
        controller = m_ControllerManager->createController(desc);
      } else {
        Yeager::Log(ERROR, "PhysX Create Controller, desc is invalid!");
      }
    } break;
    default:
      Yeager::Log(ERROR, "PhysX Character Controller shape isnt valid!");
  }
  return controller;
}

void PhysXCharacterController::ResizeController(physx::PxController* controller, physx::PxF32 capsuleRadius,
                                                physx::PxF32 capsuleHeight)
{
  controller->resize(capsuleHeight);
}

void PhysXCharacterController::Delete()
{
  m_ControllerManager->purgeControllers();
  m_ControllerManager->release();
}

void PhysXCharacterController::SetPosition(physx::PxController* controller, const physx::PxExtendedVec3& position)
{
  controller->setPosition(position);
}

PhysXCollisionDetection PhysXCharacterController::Move(physx::PxController* controller, const PxVec3& disp,
                                                       physx::PxF32 minDist, physx::PxF32 elapsedTime,
                                                       const physx::PxControllerFilters& filters,
                                                       const physx::PxObstacleContext* obstacles)
{
  PhysXCollisionDetection collisionFlags;
  collisionFlags.ExtractPhysXFlagToCollisions(controller->move(disp, minDist, elapsedTime, filters, obstacles));
  return collisionFlags;
}
