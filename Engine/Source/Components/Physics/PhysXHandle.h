//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
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

#include "Common/Math/Mathematics.h"
#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"
#include "Components/Kernel/Hardware/HardwareInfo.h"

#include "PhysXCharacterController.h"
#include "PhysXGeometryHandle.h"
#include "PhysxAllocator.h"

#define PVD_HOST "127.0.0.1"

namespace Yeager {
class ApplicationCore;

extern physx::PxVec3 Vector3ToPxVec3(const Vector3& vec);
extern Vector3 PxVec3ToVector3(const physx::PxVec3& vec);

extern physx::PxMat44 Matrix4ToPxMat44(const Matrix4& mat);
extern Matrix4 PxMat4ToMatrix4(const physx::PxMat44& mat);

class YgPxErrorCallback : public physx::PxErrorCallback {
 public:
  virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
  {
    Yeager::Log(ERROR, "PhysX error callback! Line {}, File {}, Message {}", line, file, message);
  }
};

class PhysXHandle {
 public:
  PhysXHandle(Yeager::ApplicationCore* app);

  bool InitPxEngine();
  void TerminateEngine();

  bool IsInitialized() const { return m_Initialized; }
  bool IsPxExtensionsEnabled() const { return m_PxExtensionsEnabled; }
  bool IsPxPvdEnabled() const { return m_PxPvdEnabled; }

  /**
   @brief Push the actor the PxScene and to the application
   */
  void PushToScene(physx::PxRigidActor* actor);

  YEAGER_NODISCARD physx::PxScene* GetPxScene()
  {
    if (!m_Initialized) {
      Yeager::Log(WARNING, "PhysXHandle had not been initialized yet! Crash might happen!");
    }
    return m_PxScene;
  }

  YEAGER_NODISCARD physx::PxPhysics* GetPxPhysics()
  {
    if (!m_Initialized) {
      Yeager::Log(WARNING, "PhysXHandle had not been initialized yet! Crash might happen!");
    }
    return m_PxPhysics;
  }

  void StartSimulation(float deltaTime);
  void EndSimulation();

  std::vector<Yeager::PhysXCapsule*>* GetCapsules() { return &m_Capsules; }
  std::vector<PhysXTriangleMesh*>* GetTrianglesMeshes() { return &m_TriangleMeshes; }

  Yeager::PhysXGeometryHandle* GetGeometryHandle() { return m_PhysXGeometryHandle; }

  /**
   * The engine physics engine must keep track about pointer from physX, or otherwise it can be overrided by the system
   * \return Pointer to vector of PxRigidActor pointers
   */
  YEAGER_FORCE_INLINE std::vector<physx::PxRigidActor*>* GetActorsHandle() { return &m_ActorHandle; }

  YEAGER_FORCE_INLINE Yeager::PhysXCharacterController* GetCharacterController() { return m_CharacterController; }

 private:
  physx::PxFoundation* m_PxFoundation = YEAGER_NULLPTR;
  physx::PxPhysics* m_PxPhysics = YEAGER_NULLPTR;
  physx::PxScene* m_PxScene = YEAGER_NULLPTR;
  physx::PxSceneDesc* m_PxSceneDesc = YEAGER_NULLPTR;
  physx::PxCpuDispatcher* m_PxCpuDispatcher = YEAGER_NULLPTR;
  physx::PxPvd* m_PxPvd = YEAGER_NULLPTR;
  physx::PxPvdTransport* m_PxPvdTransport = YEAGER_NULLPTR;
  physx::PxPvdSceneClient* m_PxPvdClient = YEAGER_NULLPTR;
  Yeager::PhysXGeometryHandle* m_PhysXGeometryHandle = YEAGER_NULLPTR;
  Yeager::PhysXCharacterController* m_CharacterController = YEAGER_NULLPTR;
  YgPxErrorCallback m_PxErrorCallback;
  YgPxAllocatorCallback m_PxAllocatorCallback;
  std::vector<physx::PxActor*> m_PxActors;
  bool m_Initialized = false;
  bool m_PxExtensionsEnabled = true;
  bool m_PxPvdEnabled = true;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::vector<PhysXCapsule*> m_Capsules;
  std::vector<PhysXTriangleMesh*> m_TriangleMeshes;

  std::vector<physx::PxRigidActor*> m_ActorHandle;
};
}  // namespace Yeager