//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023
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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"
#include "../Renderer/Entity.h"
#include "PhysXHandle.h"

namespace Yeager {

class Object;

struct ObjectPhysicsType {
  /**
   * Dynamic Body: It collides and moves 
   * Static Body: It collides but wont move
   * Undefined: Doesnt collide or move 
   */
  enum Enum { eDYNAMIC_BODY, eSTATIC_BODY, eUNDEFINED };
};

/**
 * @brief ObjectPhysXCreationBase is a helper class that is passed when generating a geometry for a object. It helps in the creation of the
 * physics values of the same object. This helper is the base for other helpers, it contains a type that indicate which type of body the object contains
 * 
 * @param type Body Type
 * @param position Initial Position
 * @param rotation Initial Rotation
 * @param scale Initial Scale
  */
struct ObjectPhysXCreationBase {
  ObjectPhysicsType::Enum Type = ObjectPhysicsType::eUNDEFINED;
  physx::PxVec3 Position = physx::PxVec3(0.0f);
  physx::PxVec3 Scale = physx::PxVec3(1.0f);
  physx::PxVec3 Rotation = physx::PxVec3(0.0f);
  ObjectPhysXCreationBase(const ObjectPhysicsType::Enum type = ObjectPhysicsType::eUNDEFINED,
                          const physx::PxVec3& position = physx::PxVec3(0.0f),
                          const physx::PxVec3& rotation = physx::PxVec3(0.0f),
                          const physx::PxVec3& scale = physx::PxVec3(1.0f))
      : Type(type), Position(position), Rotation(rotation), Scale(scale)
  {}
  ObjectPhysXCreationBase(const ObjectPhysicsType::Enum type = ObjectPhysicsType::eUNDEFINED,
                          const Vector3& position = Vector3(0.0f), const Vector3& rotation = Vector3(0.0f),
                          const Vector3& scale = Vector3(1.0f))
      : Type(type)
  {
    Position = Vector3ToPxVec3(position);
    Rotation = Vector3ToPxVec3(rotation);
    Scale = Vector3ToPxVec3(scale);
  }

  void ApplyToObjectTransformation(Yeager::Transformation* trans) const
  {
    trans->position = PxVec3ToVector3(Position);
    trans->rotation = PxVec3ToVector3(Rotation);
    trans->scale = PxVec3ToVector3(Scale);
  }
};

/**
 * @brief ObjectPhysXCreationStatic is a helper class that is passed when generating a geometry for a object. It helps in the creation of the
 * physics values of the same object. This helper is for static body objects, that collides with the enviroment, but doesnt moves or is affected by gravity
 * 
 * @param position Initial Position
 * @param rotation Initial Rotation
 * @param scale Initial Scale
  */
struct ObjectPhysXCreationStatic : ObjectPhysXCreationBase {
  ObjectPhysXCreationStatic(const physx::PxVec3& position = physx::PxVec3(0.0f),
                            const physx::PxVec3& rotation = physx::PxVec3(0.0f),
                            const physx::PxVec3& scale = physx::PxVec3(1.0f))
      : ObjectPhysXCreationBase(ObjectPhysicsType::eSTATIC_BODY, position, rotation, scale)
  {}
  ObjectPhysXCreationStatic(const Vector3& position = Vector3(0.0f), const Vector3& rotation = Vector3(0.0f),
                            const Vector3& scale = Vector3(1.0f))
      : ObjectPhysXCreationBase(ObjectPhysicsType::eSTATIC_BODY, position, rotation, scale)
  {}
};

/**
 * @brief ObjectPhysXCreationDynamic is a helper class that is passed when generating a geometry for a object. It helps in the creation of the
 * physics values of the same object. This helper is for dynamic body objects, that collides with the enviroment, and moves around, making interactions with other objects.
 * It alsos can be affected by gravity if requested (default is true)
 * 
 * @param position Initial Position
 * @param rotation Initial Rotation
 * @param scale Initial Scale
 * @param velocity Initial Velocity
 * @param mass Initial Mass
 * @param torque Initial Torque
*/
struct ObjectPhysXCreationDynamic : ObjectPhysXCreationBase {
  ObjectPhysXCreationDynamic(const physx::PxVec3& position = physx::PxVec3(0.0f),
                             const physx::PxVec3& rotation = physx::PxVec3(0.0f),
                             const physx::PxVec3& scale = physx::PxVec3(1.0f),
                             const physx::PxVec3& velocity = physx::PxVec3(0.0f),
                             const physx::PxReal& mass = physx::PxReal(1.0f),
                             const physx::PxVec3 torque = physx::PxVec3(0.0f))
      : ObjectPhysXCreationBase(ObjectPhysicsType::eDYNAMIC_BODY, position, rotation, scale),
        Mass(mass),
        Velocity(velocity),
        Torque(torque)
  {}

  ObjectPhysXCreationDynamic(const Vector3& position = Vector3(0.0f), const Vector3& rotation = Vector3(0.0f),
                             const Vector3& scale = Vector3(1.0f), const Vector3& velocity = Vector3(0.0f),
                             uint64_t mass = 1, const Vector3& torque = Vector3(0.0f))
      : ObjectPhysXCreationBase(ObjectPhysicsType::eDYNAMIC_BODY, position, rotation, scale)
  {
    Mass = physx::PxReal(mass);
    Velocity = Vector3ToPxVec3(velocity);
    Torque = Vector3ToPxVec3(torque);
  }

  physx::PxVec3 Velocity = physx::PxVec3(0.0f);
  physx::PxReal Mass = physx::PxReal(1.0f);
  physx::PxVec3 Torque = physx::PxVec3(0.0f);
};

class PhysXActor {
 public:
  PhysXActor(Yeager::ApplicationCore* application, Yeager::Object* object);
  ~PhysXActor();

  void BuildActor(const ObjectPhysXCreationBase& creation);
  void ProcessTransformation(float delta);

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  Yeager::Object* m_Object = YEAGER_NULLPTR;
  physx::PxRigidActor* m_Actor = YEAGER_NULLPTR;
  physx::PxMaterial* m_Material = YEAGER_NULLPTR;
};
}  // namespace Yeager
