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
class ApplicationCore;

struct RequestTypeHdl {
  enum Enum { RebuildGLFWWindow, RebuildUIInterface };
  String ToString(Enum type);
};

class RequestHandle {
 public:
  RequestHandle(Yeager::ApplicationCore* application);
  /* Calls receive a type of request to handle, if it validates has valid, it will trigger a boolean that, when the HandleRequests is called,
  all the peding requests are handle properlly */
  bool Call(RequestTypeHdl::Enum type);
  /* Discarding a request is basically removing it for being execute, this call must happen within the frame itself, it returns a boolean
  represeting if the process of turning off the request was successed, it returns false if the request queue for discard has not called */
  bool Discard(RequestTypeHdl::Enum type);
  /* Handle all request in the queue to be execute in order of importance */
  void HandleRequests();
  /* clears all the queue requests */
  void ClearAll();

 private:
  Yeager::ApplicationCore* m_Application = YEAGER_NULLPTR;

  /* Boolean and it functions, the boolean stores a value that say if the function must be called during handle request. 
  Format: 
  - The boolean must be m_Queue<Name of the RequestTypeHdl enum> like m_QueueRebuildGLFWWindow
  - The function must be Handle<Name of the RequestTypeHdl enum> like HandleRebuildGLFWWindow() it must always return a boolean 
  represeting success in the operation!
  */

  bool m_QueueRebuildGLFWWindow = false;
  bool HandleRebuildGLFWWindow();

  bool m_QueueRebuildUIInterface = false;
  bool HandleRebuildUIInterface();
};
}  // namespace Yeager
