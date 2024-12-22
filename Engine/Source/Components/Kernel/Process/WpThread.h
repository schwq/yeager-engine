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
#include "Components/Kernel/Memory/Allocator.h"

namespace Yeager {

#define DEBUG_THREAD
#define YEAGER_THREAD_DEFAULT_NAME "DNThread"

/**
 * @brief Wrapper around std::thread, contains the process name, the thread handle, and std::atomic<bool> to know when is finished.
 *  This kind of thread, when created using the ThreadManagemenet, is stored in a vector for futher debugging 
 * @attention This struct is only useful when the thread must not return a value! Only void functions can be used, here the result is compute inside the function
 */
struct WpThread {
  ~WpThread()
  {
    if (mThread.joinable())
      mThread.join();
  }
  String mProcessName = YEAGER_THREAD_DEFAULT_NAME;
  std::thread mThread;
  std::atomic<bool> bIsFinished = false;
};

/**
 * @brief A better wrapper on std::thread, its the same as WpThread but holds a std::promise, std::future and works around a value T data
 * @attention The component that make uses of this thread must take care of the asnyc value being generate, like checks to know when the thread is finished
 */
template <typename T>
struct WpThreadFuture : public WpThread {
  std::promise<T> mPromise;
  std::future<T> mFuture;

  T Get() { mFuture.get(); }
  std::atomic<bool> IsFinished() { mFuture.valid() ? bIsFinished : false; }

  T mData;
};

/**
 * @brief Thread Management managers mostly of the threads and provide function to create them.
 * All threads created using this function are returned as shared_ptr, so them dont go out of scope and terminate
 */
class ThreadManagement {
 public:
  using shared_t = std::shared_ptr<WpThread>;

  template <typename T>
  using shared_tf = std::shared_ptr<WpThreadFuture<T>>;

  /**
   * @brief Creates a new thread with the process name and a given Callable function, like a lambda function. 
   * During the thread, the function is executed and marked as finished once done. 
   * @return Returns a shared_ptr to the created thread, but dont worry, the thread is stored in a std::vector during execution
   */
  template <typename Callable>
  static shared_t NewThread(const String& process, Callable&& fun)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mProcessName = process;
    wt->mThread = std::thread([&, wt] {
      fun();
      wt->bIsFinished = true;
      ThreadDebugLog(INFO, "Thread {} was finished doing the task!", wt->mProcessName);
    });
    sNumOfThreadsCreated += 1;

    ThreadDebugLog(INFO, "Created new thread ({})!", process);

    return wt;
  }

  /**
   * @brief Creates a new thread with the process name and a given Fun function with the given Agrs arguments. 
   * During the thread, the function is executed and marked as finished once done. 
   * @return Returns a shared_ptr to the created thread, but dont worry, the thread is stored in a std::vector during execution
   */
  template <typename Fun, typename... Args>
  static shared_t NewThread(const String& process, Fun&& fun, Args&&... args)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mProcessName = process;
    wt->mThread = std::thread([&, wt] {
      fun(std::forward<Args>(args)...);
      wt->bIsFinished = true;
      ThreadDebugLog(INFO, "Thread {} was finished doing the task!", wt->mProcessName);
    });
    sNumOfThreadsCreated += 1;

    ThreadDebugLog(INFO, "Created new thread ({})!", process);

    return wt;
  }

  /**
   * @brief Creates a new thread future with the process name and a given Callable function, like a lambda function. 
   * During the thread, the function is executed and marked as finished once done. You can get the value once the thread is finished.
   * @return Returns a shared_ptr to the created thread marked as [[nodiscard]], you must take care of the thread during execution!
   */
  template <typename T, typename Callable>
  YEAGER_NODISCARD static shared_tf<T> NewThreadFuture(const String& process, Callable&& fun)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThreadFuture<T>>();
    wt->mProcessName = process;
    wt->mFuture = wt->mPromise.get_future();

    wt->mThread = std::thread([&, wt] {
      wt->mData = fun();
      wt->mPromise.set_value(wt->mData);
      wt->bIsFinished = true;
      ThreadDebugLog(INFO, "Thread {} was finished doing the task!", wt->mProcessName);
    });

    return wt;
  }

  /**
   * @brief Creates a new thread future with the process name and a given Fun function and Args arguments. 
   * During the thread, the function is executed and marked as finished once done. You can get the value once the thread is finished.
   * @return Returns a shared_ptr to the created thread marked as [[nodiscard]], you must take care of the thread during execution!
   */
  template <typename T, typename Fun, typename... Args>
  YEAGER_NODISCARD static shared_tf<T> NewThreadFuture(const String& process, Fun&& fun, Args&&... args)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThreadFuture<T>>();
    wt->mProcessName = process;
    wt->mFuture = wt->mPromise.get_future();

    wt->mThread = std::thread([&, wt] {
      wt->mData = fun(std::forward<Args>(args)...);
      wt->mPromise.set_value(wt->mData);
      wt->bIsFinished = true;
      ThreadDebugLog(INFO, "Thread {} was finished doing the task!", wt->mProcessName);
    });

    return wt;
  }

  template <typename... T>
  static void ThreadDebugLog(int verbosity, fmt::format_string<T...> fmt, T&&... args)
  {
#ifdef DEBUG_THREAD
    Yeager::LogDebug(verbosity, fmt, std::forward<T>(args)...);
#endif
  }

 private:
  static std::size_t sNumOfThreadsCreated;
};
}  // namespace Yeager