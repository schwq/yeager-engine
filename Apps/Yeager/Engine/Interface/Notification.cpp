#include "Notification.h"
using namespace Yeager;
using namespace ImGui;
NotificationBox::NotificationBox(YgString title, YgString message, ImVec4 text_color, YgString icon,

                                 unsigned int time_appearing)
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

void NotificationBox::Appear(unsigned int size_x, unsigned int size_y, unsigned int pos_x, unsigned int pos_y) {}