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

struct WpThread {
  String mProcessName = "DNThread";
  std::thread mThread;
};

class ThreadManagement {
 public:
  using shared_t = std::shared_ptr<WpThread>;
  /*
  template <typename Fun, typename... Args>
  static shared_t NewThread(Fun&& fun, Args&&... args)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mThread = std::thread(fun, std::forward<Args>(args)...);
    sThreadsInUse.push_back(wt);

#ifdef DEBUG_THREAD
    Yeager::LogDebug(INFO, "Created new thread ({})! Current number of threads: {}", wt->mProcessName,
                     sThreadsInUse.size());
#endif

    return wt;
  }

  template <typename Callable>
  static shared_t NewThread(Callable&& fun)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mThread = std::thread([=] { fun(); });
    sThreadsInUse.push_back(wt);

#ifdef DEBUG_THREAD
    Yeager::LogDebug(INFO, "Created new thread ({})! Current number of threads: {}", wt->mProcessName,
                     sThreadsInUse.size());
#endif

    return wt;
  }
*/
  template <typename Callable>
  static shared_t NewThread(const String& process, Callable&& fun)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mProcessName = process;
    wt->mThread = std::thread([=] { fun(); });
    sThreadsInUse.push_back(wt);

#ifdef DEBUG_THREAD
    Yeager::LogDebug(INFO, "Created new thread ({})! Current number of threads: {}", wt->mProcessName,
                     sThreadsInUse.size());
#endif

    return wt;
  }

  template <typename Fun, typename... Args>
  static shared_t NewThread(const String& process, Fun&& fun, Args&&... args)
  {
    auto wt = BaseAllocator::MakeSharedPtr<WpThread>();
    wt->mProcessName = process;
    wt->mThread = std::thread(fun, std::forward<Args>(args)...);
    sThreadsInUse.push_back(wt);

#ifdef DEBUG_THREAD
    Yeager::LogDebug(INFO, "Created new thread ({})! Current number of threads: {}", wt->mProcessName,
                     sThreadsInUse.size());
#endif

    return wt;
  }

  static void TerminateThreads()
  {
    std::size_t num = sThreadsInUse.size();
    for (auto& t : sThreadsInUse) {
      if (t->mThread.joinable())
        t->mThread.join();
#ifdef DEBUG_THREAD
      Yeager::LogDebug(INFO, "Terminated thread ({})! Current number of threads: {}", t->mProcessName, --num);
#endif
      t.reset();
    }
    sThreadsInUse.clear();
  }

 private:
  static std::vector<shared_t> sThreadsInUse;
};
}  // namespace Yeager