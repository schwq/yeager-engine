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
  yg_string message;
  yg_string title;
  ImVec4 text_color;
  yg_string icon;
};

class NotificationBox {
 public:
  NotificationBox(yg_string title, yg_string message, ImVec4 text_color = IMGUI_WHITE_COMMON_COLOR,
                  yg_string icon = ICON_FA_NOTE_STICKY, yg_uint time_appearing = 15);
  ~NotificationBox();

  void Appear(yg_uint size_x, yg_uint size_y, yg_uint pos_x, yg_uint pos_y);

 private:
  NotificationBody m_body;
  yg_uint m_time = 15;
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