//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/yeager-engine
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
#include "../../Kernel/HardwareInfo.h"

#include "PhysxAllocator.h"

#define PVD_HOST "127.0.0.1"

namespace Yeager {

extern physx::PxVec3 YgVector3ToPxVec3(const YgVector3& vec);
extern YgVector3 PxVec3ToYgVector3(const physx::PxVec3& vec);

class YgPxErrorCallback : public physx::PxErrorCallback {
 public:
  virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
  {
    Yeager::Log(ERROR, "PhysX error callback! Line {}, File {}, Message {}", line, file, message);
  }
};

class PhysXHandle {
 public:
  PhysXHandle() {}
  bool InitPxEngine();
  void TerminateEngine();

  bool IsInitialized() const { return m_Initialized; }
  bool IsPxExtensionsEnabled() const { return m_PxExtensionsEnabled; }
  bool IsPxPvdEnabled() const { return m_PxPvdEnabled; }

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

 private:
  physx::PxFoundation* m_PxFoundation = YEAGER_NULLPTR;
  physx::PxPhysics* m_PxPhysics = YEAGER_NULLPTR;
  physx::PxScene* m_PxScene = YEAGER_NULLPTR;
  physx::PxSceneDesc* m_PxSceneDesc = YEAGER_NULLPTR;
  physx::PxCpuDispatcher* m_PxCpuDispatcher = YEAGER_NULLPTR;
  physx::PxPvd* m_PxPvd = YEAGER_NULLPTR;
  physx::PxPvdTransport* m_PxPvdTransport = YEAGER_NULLPTR;
  physx::PxPvdSceneClient* m_PxPvdClient = YEAGER_NULLPTR;
  YgPxErrorCallback m_PxErrorCallback;
  YgPxAllocatorCallback m_PxAllocatorCallback;
  std::vector<physx::PxActor*> m_PxActors;
  bool m_Initialized = false;
  bool m_PxExtensionsEnabled = true;
  bool m_PxPvdEnabled = true;
};
}  // namespace Yeager