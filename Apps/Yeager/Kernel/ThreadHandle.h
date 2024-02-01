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

#include "../Application.h"
#include "../Common/Common.h"
#include "../Common/LogEngine.h"
#include "../Common/Utilities.h"

#define YEAGER_THREAD_PROCESS_DEFAULT_NAME "ProcessDefault"

/* TODO finish this */

namespace Yeager {

class ApplicationCore;

template <class Type>
class ThreadHandle {
 public:
  template <class Type>
  ThreadHandle(YgString processName, Yeager::ApplicationCore* app) : m_ProcessName(processName), m_Application(app)
  {
    m_FutureType = m_PromiseType.get_future();
    Yeager::Log(INFO, "Thread {} created!", processName);
  }

  template <class Type>
  void StartThread(std::thread& process)
  {
    m_Thread = process;
  }

  std::thread* GetThreadPtr() { return &m_Thread; }

  YEAGER_FORCE_INLINE std::atomic<bool> IsFinished() const
  {
      if (!m_FutureType.valid() {
      Yeager::Log(WARNING, "Thread {} future object isnt valid!", m_ProcessName);
      return false;
      }												
	  return m_ThreadFinished;
  }
  Type GetValue() { return m_FutureType.get(); }

 protected:
  YgString m_ProcessName = YEAGER_THREAD_PROCESS_DEFAULT_NAME;
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;
  std::promise<Type> m_PromiseType;
  std::future<Type> m_FutureType;
  std::thread m_Thread;
  std::atomic<bool> m_ThreadFinished = false;
};

};  // namespace Yeager
