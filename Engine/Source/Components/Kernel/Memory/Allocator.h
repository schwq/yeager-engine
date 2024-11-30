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

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

namespace Yeager {

struct PointerTypeStats {
  uint mCount = 0;
};

struct GeneralMemoryAllocationStats {
  uint mAllocations = 0;
  uint mDealocations = 0;

  uint mNewOperatorCalls = 0;
  uint mDeleteOperatorCalls = 0;

  std::size_t mBytesAllocated = 0;
  std::size_t mBytesDeallocated = 0;

  const std::size_t DiffOfBytes() noexcept { return mBytesAllocated - mBytesDeallocated; }
};

class BaseAllocator {
 public:
  template <typename T>
  using weak = std::weak_ptr<T>;

  template <typename T>
  using shared = std::shared_ptr<T>;

  template <typename T>
  using unique = std::unique_ptr<T>;

  static PointerTypeStats sSharedPointersCreated;
  static PointerTypeStats sUniquePointersCreated;
  static PointerTypeStats sWeakPointersCreated;

  static GeneralMemoryAllocationStats sGenMemoryStats;

  template <typename T>
  static T* Allocate(std::size_t n = 1)
  {
    sGenMemoryStats.mNewOperatorCalls += 1;
    sGenMemoryStats.mAllocations += 1;
    sGenMemoryStats.mBytesAllocated += n;
    T* ptr = static_cast<T*>(::operator new(n * sizeof(T)));
    mPointersInUse[static_cast<void*>(ptr)] = n * sizeof(T);

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Successful allocated size {}, located {}", n * sizeof(T), fmt::ptr(ptr));
#endif

    return ptr;
  }

  template <typename T>
  static void Deallocate(T* ptr) noexcept
  {
    sGenMemoryStats.mDeleteOperatorCalls += 1;
    sGenMemoryStats.mDealocations += 1;
    std::size_t sz = mPointersInUse[static_cast<void*>(ptr)];
    sGenMemoryStats.mBytesDeallocated += sz;
    mPointersInUse.erase(static_cast<void*>(ptr));

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Successful deallocated size {}, located {}", sz, fmt::ptr(ptr));
#endif

    YEAGER_DELETE_OP(ptr);
  }

  template <typename T, typename... Args>
  static T* Construct(Args&&... args)
  {
    sGenMemoryStats.mNewOperatorCalls += 1;
    T* ptr = Allocate<T>(1);
    new (ptr) T(std::forward<Args>(args)...);

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Successful construct size {}, located {}", sizeof(T), fmt::ptr(ptr));
#endif

    return ptr;
  }

  template <typename T>
  static void Destroy(T* ptr) noexcept
  {
    ptr->~T();

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Successful destroyed size {}, located {}", sizeof(T), fmt::ptr(ptr));
#endif
  }

  /**
   * @brief Involkes std::unque_ptr<T>(__args...), write information about it and debug if defined DEBUG_MEM
   */
  template <typename T, typename... _Args>
  static YEAGER_FORCE_INLINE weak<T> NewWeakPtr(_Args&&... __args)
  {
    weak ptr = weak<T>(__args...);
    BaseAllocator::sWeakPointersCreated.mCount += 1;

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Succcess allocated weak_ptr size: {}, located: {}", sizeof(T), fmt::ptr(ptr.get()));
#endif

    return ptr;
  }

  /**
   * @brief Involkes std::make_unique<T>(__args...), write information about it and debug if defined DEBUG_MEM
   */
  template <typename T, typename... _Args>
  static YEAGER_FORCE_INLINE unique<T> MakeUniquePtr(_Args&&... __args)
  {
    unique ptr = std::make_unique<T>(__args...);
    BaseAllocator::sUniquePointersCreated.mCount += 1;

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Succcess allocated unique_ptr size: {}, located: {}", sizeof(T), fmt::ptr(ptr.get()));
#endif

    return ptr;
  }

  /**
   * @brief Involkes std::make_shared<T>(__args...), write information about it and debug if defined DEBUG_MEM
   */
  template <typename T, typename... _Args>
  static YEAGER_FORCE_INLINE shared<T> MakeSharedPtr(_Args&&... __args)
  {
    shared ptr = std::make_shared<T>(__args...);
    BaseAllocator::sSharedPointersCreated.mCount += 1;

#ifdef DEBUG_MEM
    Yeager::LogDebug(INFO, "Succcess allocated shared_ptr size: {}, located: {}", sizeof(T), fmt::ptr(ptr.get()));
#endif

    return ptr;
  }

 private:
  static std::unordered_map<void*, std::size_t> mPointersInUse;
};

}  // namespace Yeager