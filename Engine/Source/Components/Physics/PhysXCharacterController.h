//   Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "PhysxAllocator.h"

/* We cant spawn inside the floor! This variable should only be used went the engine doesnt know where to spawn the character! */
#define YEAGER_DEFAULT_CAMERA_POSITION Vector3(0.0f, 2.0f, 0.0f);
#define YEAGER_PHYSX_CCT_VS_CCT_COLLIDE_AND_SLIDE true

struct PhysXCollisionDetection {
  bool bHasCollideAbove = false;
  bool bHasCollideBelow = false;
  bool bHasCollideSides = false;
  YEAGER_FORCE_INLINE void ExtractPhysXFlagToCollisions(const physx::PxControllerCollisionFlags& flags)
  {
    if (flags & physx::PxControllerCollisionFlag::eCOLLISION_UP)
      bHasCollideAbove = true;
    if (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
      bHasCollideBelow = true;
    if (flags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES)
      bHasCollideSides = true;
  }
};

namespace Yeager {

class PhysXHandle;

class PhysXCharacterController : physx::PxUserControllerHitReport,
                                 physx::PxControllerBehaviorCallback,
                                 physx::PxControllerFilterCallback {

 public:
  PhysXCharacterController(PhysXHandle* handle);
  ~PhysXCharacterController();

  void Create();
  void Delete();

  /* Create Controller adds the controller to the controller manager, with the created description, and returns a pointer
  to the controller if the developer wants to tweak a little futher with the newest created controller */
  physx::PxController* CreateController(
      const physx::PxControllerShapeType::Enum type = physx::PxControllerShapeType::eCAPSULE,
      const physx::PxF32 stepOffSet = 0.4f, physx::PxF32 capsuleRadius = 1.0f, physx ::PxF32 capsuleHeight = 2.0f);

  void ResizeController(physx::PxController* controller, physx::PxF32 capsuleRadius, physx::PxF32 capsuleHeight);

  void SetPosition(physx::PxController* controller, const physx::PxExtendedVec3& position);

  PhysXCollisionDetection Move(physx::PxController* controller, const physx::PxVec3& disp, physx::PxF32 minDist,
                               physx::PxF32 elapsedTime, const physx::PxControllerFilters& filters,
                               const physx::PxObstacleContext* obstacles = YEAGER_NULLPTR);

  virtual void onControllerHit(const physx::PxControllersHit& hit) { Yeager::Log(INFO, "Controller Hit!"); }
  virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) { Yeager::Log(INFO, "Obstacle Hit!"); }
  virtual void onShapeHit(const physx::PxControllerShapeHit& hit)
  {
    physx::PxActorType::Enum type = hit.actor->getType();
    if (type == physx::PxActorType::eRIGID_DYNAMIC) {
      Yeager::Log(INFO, "Shape Hit!");
      physx::PxRigidDynamic* ac = (physx::PxRigidDynamic*)hit.actor;
      ac->addForce(hit.dir * 50.0f);
    }
  }

  virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor)
  {
    physx::PxControllerBehaviorFlags flags;
    flags |= physx::PxControllerBehaviorFlag::eCCT_SLIDE;
    return flags;
  }

  virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxController& controller)
  {
    physx::PxControllerBehaviorFlags
        flags;  // getBehaviorFlags for controller does not support the eCCT_CAN_RIDE_ON_OBJECT flag
    flags |= physx::PxControllerBehaviorFlag::eCCT_USER_DEFINED_RIDE;
    return flags;
  }

  virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxObstacle& obstacle)
  {
    physx::PxControllerBehaviorFlags flags;
    flags |= physx::PxControllerBehaviorFlag::eCCT_SLIDE;
    return flags;
  }

  /* To make CCTs always collide-and-slide against each other, make it return true. 
  To make CCTs always move freely through each other, make it return false. */
  virtual bool filter(const physx::PxController& a, const physx::PxController& b)
  {
    return YEAGER_PHYSX_CCT_VS_CCT_COLLIDE_AND_SLIDE;
  }

  YEAGER_FORCE_INLINE physx::PxObstacleHandle AddBoxObstacle(const physx::PxExtendedVec3& position,
                                                             const physx::PxQuat& rotation,
                                                             const physx::PxVec3& halfExtents)
  {
    physx::PxBoxObstacle obstacle;
    obstacle.mPos = position;
    obstacle.mRot = rotation;
    obstacle.mHalfExtents = halfExtents;
    return m_ObstacleContext->addObstacle(obstacle);
  }

  YEAGER_FORCE_INLINE physx::PxObstacleHandle AddCapsuleObstacle(const physx::PxExtendedVec3& position,
                                                                 const physx::PxQuat& rotation,
                                                                 const physx::PxReal& halfHeight,
                                                                 const physx::PxReal& radius)
  {
    physx::PxCapsuleObstacle obstacle;
    obstacle.mPos = position;
    obstacle.mRot = rotation;
    obstacle.mHalfHeight = halfHeight;
    obstacle.mRadius = radius;
    return m_ObstacleContext->addObstacle(obstacle);
  }

  /* The engine camera used in the editor mode (EditorCamera class) can be used as a player camera with some
  limitations and tweaks TODO write a PlayerCamera class inheranted from the editor camera */

  YEAGER_FORCE_INLINE physx::PxControllerManager* GetControllerManager() { return m_ControllerManager; }

  YEAGER_FORCE_INLINE physx::PxObstacleContext* GetObstacleContext() { return m_ObstacleContext; }

 protected:
  Yeager::PhysXHandle* m_PhysXHandle = YEAGER_NULLPTR;
  physx::PxControllerManager* m_ControllerManager = YEAGER_NULLPTR;
  physx::PxObstacleContext* m_ObstacleContext = YEAGER_NULLPTR;
};

};  // namespace Yeager
