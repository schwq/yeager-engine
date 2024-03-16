#include "Request.h"
#include "Application.h"
using namespace Yeager;

String RequestTypeHdl::ToString(RequestTypeHdl::Enum type)
{
  switch (type) {
    case RequestTypeHdl::RebuildGLFWWindow:
      return "Rebuild GLFW window";
    case RequestTypeHdl::RebuildUIInterface:
      return "Rebuild UI Interface";
    default:
      return "Invalid Request Type";
  }
}

RequestHandle::RequestHandle(Yeager::ApplicationCore* application) : m_Application(application) {}

bool RequestHandle::Call(RequestTypeHdl::Enum type)
{
  switch (type) {
    case RequestTypeHdl::RebuildGLFWWindow:
      m_QueueRebuildGLFWWindow = true;
      return true;
    case RequestTypeHdl::RebuildUIInterface:
      /* The rebuild glfw window will trigger the rebuild ui, so we dont want to build a ui twice! */
      if (!m_QueueRebuildGLFWWindow)
        m_QueueRebuildUIInterface = true;
      return true;
    default:
      Yeager::Log(WARNING, "Request Call Type invalid!");
      return false;
  }
}

bool RequestHandle::Discard(RequestTypeHdl::Enum type)
{
  switch (type) {
    case RequestTypeHdl::RebuildGLFWWindow:
      if (m_QueueRebuildGLFWWindow) {
        m_QueueRebuildGLFWWindow = false;
        return true;
      } else {
        return false;
      }

    case RequestTypeHdl::RebuildUIInterface:

      if (m_QueueRebuildUIInterface) {
        m_QueueRebuildUIInterface = false;
        return true;
      } else {
        return false;
      }
      break;

    default:
      Yeager::Log(WARNING, "Request call for discard was set or is invalid!");
      return false;
  }
}

void RequestHandle::ClearAll()
{
  m_QueueRebuildGLFWWindow = false;
  m_QueueRebuildUIInterface = false;
}

void RequestHandle::HandleRequests()
{
  if (m_QueueRebuildGLFWWindow) {
    if (!HandleRebuildGLFWWindow()) {
      Yeager::Log(ERROR, "Cannot handle the rebuild glfw window request!");
    }
    m_QueueRebuildGLFWWindow = false;
  }
  if (m_QueueRebuildUIInterface) {
    if (!HandleRebuildUIInterface()) {
      Yeager::Log(ERROR, "Cannot handle the rebuild ui interface request!");
    }
    m_QueueRebuildUIInterface = false;
  }
}

bool RequestHandle::HandleRebuildGLFWWindow()
{
  if (!m_Application->GetInterface()->IsRenderFrameBeingDraw()) {
    Yeager::Window* wnd = m_Application->GetWindow();
    Yeager::WindowInfo* info = wnd->GetWindowInformationPtr();
    bool success = true;
    if (!wnd->RegenerateMainWindow(info->Size.x, info->Size.y, info->Title, info->CursorFunc))
      success = false;
    m_Application->GetInterface()->RequestRestartInterface(wnd);

    return success;
  }
  /* This should not happen, the request is handle after the draw is done, so if it happens, it is some bad code written by me */
  return false;
}

bool RequestHandle::HandleRebuildUIInterface()
{
  if (!m_Application->GetInterface()->IsRenderFrameBeingDraw()) {
    m_Application->GetInterface()->RequestRestartInterface(m_Application->GetWindow());
    return true;
  }
  /* This should not happen, the request is handle after the draw is done, so if it happens, it is some bad code written by me */
  return false;
}
