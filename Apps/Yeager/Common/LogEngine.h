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
#include "Common.h"

enum LogSource { kUser, kSystem, kNone };

typedef struct {
  yg_string message;
  int verbosity;
  ImVec4 text_color;
} ConsoleLogSender;

extern yg_string ConsoleLogSenderToString(LogSource sender);
extern ImVec4 VerbosityToColor(int verbosity);

class EditorConsole {
 public:
  EditorConsole(){};
  ~EditorConsole(){};

  void SetLogString(ConsoleLogSender message) { m_logs.push_back(message); }

  void ReadLog();

 private:
  std::vector<ConsoleLogSender> m_logs;
};

namespace Yeager {
static EditorConsole kConsole;

template <typename... T>
void Log(int verbosity, LogSource sender, fmt::format_string<T...> fmt, T&&... args)
{
  auto str = fmt::format(fmt, std::forward<T>(args)...);
  yg_string log(str);

  ConsoleLogSender console_message;
  console_message.message = "[" + ConsoleLogSenderToString(sender) + "] " + log;
  console_message.text_color = VerbosityToColor(verbosity);
  console_message.verbosity = verbosity;

  kConsole.SetLogString(console_message);
  yg_string terminal_prefix;
  char color;
  if (verbosity == INFO) {
    terminal_prefix = "(-) ";
  } else if (verbosity == WARNING) {
    terminal_prefix = "(??) ";
  } else {
    terminal_prefix = "(!!) ";
  }

  if (!(sender == kUser)) {
    std::cout << terminal_prefix << log << std::endl;
  }
}
}  // namespace Yeager