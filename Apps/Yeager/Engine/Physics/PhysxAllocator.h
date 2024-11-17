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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "../../Common/Utilities.h"

#include <malloc.h>
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"

namespace Yeager {

#ifdef YEAGER_SYSTEM_LINUX
YEAGER_FORCE_INLINE void* platformAlignedAlloc(size_t size)
{
  return ::memalign(16, size);
}
YEAGER_FORCE_INLINE void platformAlignedFree(void* ptr)
{
  ::free(ptr);
}
#elif defined(YEAGER_SYSTEM_WINDOWS_x64)
YEAGER_FORCE_INLINE void* platformAlignedAlloc(size_t size)
{
  return _aligned_malloc(size, 16);
}
YEAGER_FORCE_INLINE void platformAlignedFree(void* ptr)
{
  _aligned_free(ptr);
}
#else
YEAGER_FORCE_INLINE void* platformAlignedAlloc(size_t size)
{
  return malloc(size, 16);
}
YEAGER_FORCE_INLINE void platformAlignedFree(void* ptr)
{
  free(ptr);
}
#endif

class YgPxAllocatorCallback : public physx::PxAllocatorCallback {
 public:
  virtual void* allocate(size_t size, const char*, const char*, int)
  {
    void* ptr = platformAlignedAlloc(size);
    PX_ASSERT((size_t(ptr) % 15) == 0);
#if PX_STOMP_ALLOCATED_MEMORY
    if (ptr != NULL) {
      physx::PxMemSet(ptr, physx::PxI32(0xcd), physx::PxU32(size));
    }
#endif
    return ptr;
  }

  virtual void deallocate(void* ptr)
  {
    platformAlignedFree(ptr);
  }
};
}  // namespace Yeager