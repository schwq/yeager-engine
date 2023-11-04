#include "Notification.h"
using namespace Yeager;
using namespace ImGui;
NotificationBox::NotificationBox(yg_string title, yg_string message, ImVec4 text_color, yg_string icon,
                                 yg_uint time_appearing)
{
  NotificationBody body;
  body.title = title;
  body.message = message;
  body.text_color = text_color;
  body.icon = icon;
  m_body = body;
  m_time = time_appearing;
}

NotificationBox::~NotificationBox() {}

void NotificationBox::Appear(yg_uint size_x, yg_uint size_y, yg_uint pos_x, yg_uint pos_y) {}