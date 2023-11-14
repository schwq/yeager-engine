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

#include "../../Common/Common.h"
#include "../../Common/LogEngine.h"
#include "IconsFontAwesome6.h"
#include "UIManagement.h"

namespace Yeager {

struct NotificationBody {
  YgString message;
  YgString title;
  ImVec4 text_color;
  YgString icon;
};

class NotificationBox {
 public:
  NotificationBox(YgString title, YgString message, ImVec4 text_color = IMGUI_WHITE_COMMON_COLOR,
                  YgString icon = ICON_FA_NOTE_STICKY,

                  unsigned int time_appearing = 15);
  ~NotificationBox();

  void Appear(unsigned int size_x, unsigned int size_y, unsigned int pos_x, unsigned int pos_y);

 private:
  NotificationBody m_body;
  unsigned int m_time = 15;
};

class NotificationEngine final {
 public:
  NotificationEngine();
  ~NotificationEngine();

  void RequestNotifications();
  void AddNotificationQuery();

 private:
  void KillNotification();

  std::vector<NotificationBox> m_notifications;
};

}  // namespace Yeager