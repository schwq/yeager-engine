#include "UIManagement.h"
using namespace ImGui;
using namespace Yeager;
InterfaceWindow::InterfaceWindow(yg_string title, ImVec2 size, InterfaceWindow* parent_window,
                                 ImVec2 parent_relative_pos, WindowRelativePos relative_pos)
    : m_title(title),
      m_size(size),
      m_parent_window(parent_window),
      m_parent_relative_pos(parent_relative_pos),
      m_relative_pos(relative_pos)
{
  m_screen_position = ImVec2(0.0f, 0.0f);
}
InterfaceWindow::InterfaceWindow(yg_string title, ImVec2 size, ImVec2 position, bool follow_glfw_window,
                                 WindowRelativePos glfw_relative_pos)
    : m_title(title),
      m_size(size),
      m_screen_position(position),
      m_follow_glfw_window(follow_glfw_window),
      m_glfw_relative_pos(glfw_relative_pos)
{
  m_root_window = true;
}
InterfaceWindow::~InterfaceWindow() {}

void InterfaceWindow::Begin()
{

  CalcScreenPos();
  SetNextWindowPos(m_screen_position);
  SetNextWindowSize(m_size);
  m_size = temp_resize;
  PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin(m_title.c_str(), NULL, kWindowStatic);
}

void InterfaceWindow::End()
{
  /// What is this doing here, well, if in the future, we want to alloc something, this is going to be used to desalloc
  ImGui::End();
  PopStyleVar();
}

void InterfaceWindow::CalcScreenPos()
{
  ImVec2 glfw_window = ImVec2(ygWindowWidth, ygWindowHeight);

  if (m_root_window) {

    if (m_follow_glfw_window) {

      switch (m_glfw_relative_pos) {
        case WindowRelativePos::LEFT_ABOVE: {
          ImVec2 pos = ImVec2(0, 0);
          pos.y += m_menu_bar_size.y;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::RIGHT_ABOVE: {
          ImVec2 pos = ImVec2(0, 0);
          pos.y += m_menu_bar_size.y;
          pos.x = glfw_window.x - m_size.x;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::ABOVE_MIDDLE: {
          yg_uint middle_above_x = glfw_window.x / 2;
          yg_uint window_middle_x = m_size.x / 2;
          ImVec2 pos = ImVec2(middle_above_x - window_middle_x, 0);
          pos.y += m_menu_bar_size.y;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::UNDER_MIDDLE: {
          yg_uint middle_above_x = glfw_window.x / 2;
          yg_uint window_middle_x = m_size.x / 2;
          ImVec2 pos = ImVec2(middle_above_x - window_middle_x, glfw_window.y - m_size.y);
          m_screen_position = pos;
        } break;
        case WindowRelativePos::LEFT_MIDDLE: {
          yg_uint middle_right_y = glfw_window.y / 2;
          yg_uint window_middle_y = m_size.y / 2;
          ImVec2 pos = ImVec2(0, middle_right_y - window_middle_y);
          pos.y += m_menu_bar_size.y;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::MIDDLE: {
          yg_uint window_middle_x = m_size.x / 2;
          yg_uint window_middle_y = m_size.y / 2;
          yg_uint middle_window_x = glfw_window.x / 2;
          yg_uint middle_window_y = glfw_window.y / 2;
          ImVec2 pos = ImVec2(middle_window_x - window_middle_x, middle_window_y - window_middle_y);
          pos.y += m_menu_bar_size.y;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::RIGHT_MIDDLE: {
          yg_uint middle_left_y = glfw_window.y / 2;
          yg_uint window_middle_y = m_size.y / 2;
          ImVec2 pos = ImVec2(glfw_window.x, middle_left_y - window_middle_y);
          pos.y += m_menu_bar_size.y;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::LEFT_UNDER: {
          ImVec2 pos = ImVec2(0, glfw_window.y - m_size.y);
          m_screen_position = pos;
        } break;
        case WindowRelativePos::RIGHT_UNDER: {
          ImVec2 pos = ImVec2(glfw_window.x - m_size.x, glfw_window.y - m_size.y);
          m_screen_position = pos;
        } break;
      }
    }
  } else {
    if (m_parent_window) {
      ImVec2 parent_pos = m_parent_window->GetScreenPos();
      ImVec2 parent_size = m_parent_window->GetSize();
      ImVec2 pos = parent_pos;
      switch (m_relative_pos) {
        case WindowRelativePos::ABOVE: {
          pos.y -= m_size.x;
          pos.y -= m_parent_relative_pos.y;
          pos.x -= m_parent_relative_pos.x;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::UNDER: {
          pos.y += parent_size.y;
          pos.y += m_parent_relative_pos.y;
          pos.x += m_parent_relative_pos.x;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::RIGHT: {
          pos.x += parent_size.x;
          pos.y += m_parent_relative_pos.y;
          pos.x += m_parent_relative_pos.x;
          m_screen_position = pos;
        } break;
        case WindowRelativePos::LEFT: {
          pos.x -= m_size.x;
          pos.y += m_parent_relative_pos.y;
          pos.x -= m_parent_relative_pos.x;
          m_screen_position = pos;
        } break;
      }
    }
  }
}

bool InterfaceWindow::GetIsRoot()
{
  return m_root_window;
}
bool InterfaceWindow::GetFollowGLFWWindow()
{
  return m_follow_glfw_window;
}
InterfaceWindow* InterfaceWindow::GetParent()
{
  return m_parent_window;
}
ImVec2 InterfaceWindow::GetParentRelativePos()
{
  return m_parent_relative_pos;
}
WindowRelativePos InterfaceWindow::GetRelativePos()
{
  return m_relative_pos;
}
ImVec2 InterfaceWindow::GetSize()
{
  return m_size;
}
ImVec2 InterfaceWindow::GetScreenPos()
{
  return m_screen_position;
}

void InterfaceWindow::SetMenuBarSize(ImVec2 size)
{
  m_menu_bar_size = size;
}

void InterfaceWindow::SetSize(ImVec2 size)
{
  m_size = size;
}
void InterfaceWindow::SetPos(ImVec2 pos)
{
  m_screen_position = pos;
}

bool InterfaceWindow::GetCollision(yg_uint x, yg_uint y)
{
  bool collision_x = false;
  bool collision_y = false;
  if (m_screen_position.x <= x && m_screen_position.x + m_size.x >= x) {
    collision_x = true;
  }
  if (m_screen_position.y <= y && m_screen_position.y + m_size.y >= y) {
    collision_y = true;
  }
  return collision_x && collision_y;
}

void InterfaceWindow::SetRule(EYGWindowRule rule, bool value)
{
  switch (rule) {
    case EWindowFullscreen:
      m_rules.fullscreen = value;
      break;
    case EWindowFollowAbove:
      m_rules.follow_above = value;
      break;
    case EWindowFollowUnder:
      m_rules.follow_under = value;
      break;
    case EWindowSameSizeParent:
      m_rules.same_size_parent = value;
      break;
    case EWindowSameWidthParent:
      m_rules.same_width_parent = value;
      break;
    case EWindowSameHeightParent:
      m_rules.same_height_parent = value;
      break;
    case EWindowRoundingCorner:
      m_rules.rounding_corner = value;
      break;
    default:
      Yeager::Log(ERROR, "Invalid window rule!");
  }
}

bool InterfaceWindow::GetRule(EYGWindowRule rule)
{
  switch (rule) {
    case EWindowFullscreen:
      return m_rules.fullscreen;
    case EWindowFollowAbove:
      return m_rules.follow_above;
    case EWindowFollowUnder:
      return m_rules.follow_under;
    case EWindowSameSizeParent:
      return m_rules.same_size_parent;
    case EWindowSameWidthParent:
      return m_rules.same_width_parent;
    case EWindowSameHeightParent:
      return m_rules.same_height_parent;
    case EWindowRoundingCorner:
      return m_rules.rounding_corner;
    default:
      Yeager::Log(ERROR, "Invalid window rule!");
  }
}