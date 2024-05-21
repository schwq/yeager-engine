//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
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
#include "Utilities.h"

struct MessageTypeVerbosity {
  enum Enum { Error_Message, Warning_Message, Info_Message };
  static Enum VerbosityToEnum(int verb)
  {
    switch (verb) {
      case 0:
        return Info_Message;
      case -1:
        return Warning_Message;
      default:
        return Error_Message;
    }
  }
};

struct ConsoleLogSender {
  String message = YEAGER_NULL_LITERAL;
  MessageTypeVerbosity::Enum type = MessageTypeVerbosity::Info_Message;
  int verbosity = -1;
  ImVec4 text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};

extern ImVec4 VerbosityToColor(int verbosity);

class EditorConsole {
 public:
  EditorConsole(){};
  ~EditorConsole(){};

  void SetLogString(ConsoleLogSender message) { m_logs.push_back(message); }

  void ReadLog();
  std::vector<ConsoleLogSender>* GetLogs() { return &m_logs; }

 private:
  std::vector<ConsoleLogSender> m_logs;
};
extern EditorConsole gGlobalConsole;

namespace Yeager {

template <typename... T>
void Log(int verbosity, fmt::format_string<T...> fmt, T&&... args)
{
  auto str = fmt::format(fmt, std::forward<T>(args)...);
  String log(str);

  ConsoleLogSender console_message;
  console_message.text_color = VerbosityToColor(verbosity);
  console_message.verbosity = verbosity;
  console_message.message = log;
  console_message.type = MessageTypeVerbosity::VerbosityToEnum(verbosity);

  String terminal_prefix;
  String console_verbose;
  if (verbosity == INFO) {
    terminal_prefix = "(-) ";
    console_verbose = "[INFO] ";
  } else if (verbosity == WARNING) {
    terminal_prefix = "(??) ";
    console_verbose = "[WARN] ";
  } else {
    terminal_prefix = "(!!) ";
    console_verbose = "[ERROR] ";
  }
  console_message.message = String(console_verbose + console_message.message);
  gGlobalConsole.SetLogString(console_message);

  YgTime_t time = CurrentTimeToTimeType();
  String time_str = "[ " + std::to_string(time.Time.Hours) + ":" + std::to_string(time.Time.Minutes) + ":" +
                    std::to_string(time.Time.Seconds) + " ]";
  std::cout << time_str << terminal_prefix << log << std::endl;
}

template <typename... T>
void LogDebug(int verbosity, fmt::format_string<T...> fmt, T&&... args)
{
#ifdef YEAGER_DEBUG
  auto str = fmt::format(fmt, std::forward<T>(args)...);
  String log(str);

  ConsoleLogSender console_message;
  console_message.text_color = VerbosityToColor(verbosity);
  console_message.verbosity = verbosity;
  console_message.message = log;
  console_message.type = MessageTypeVerbosity::VerbosityToEnum(verbosity);

  String terminal_prefix;
  String console_verbose;
  if (verbosity == INFO) {
    terminal_prefix = "(-) ";
    console_verbose = "[INFO] ";
  } else if (verbosity == WARNING) {
    terminal_prefix = "(??) ";
    console_verbose = "[WARN] ";
  } else {
    terminal_prefix = "(!!) ";
    console_verbose = "[ERROR] ";
  }
  console_message.message = String(console_verbose + console_message.message);
  gGlobalConsole.SetLogString(console_message);

  YgTime_t time = CurrentTimeToTimeType();
  String time_str = "[ " + std::to_string(time.Time.Hours) + ":" + std::to_string(time.Time.Minutes) + ":" +
                    std::to_string(time.Time.Seconds) + " ]";
  std::cout << time_str << terminal_prefix << log << std::endl;
#endif
}

#define YEAGER_CONSTRUCTOR_LOG(source)            \
  {                                               \
    Yeager::LogDebug(INFO, "Created {}", source); \
  }

#define YEAGER_DESTRUCTOR_LOG(source)                \
  {                                                  \
    Yeager::LogDebug(INFO, "Destroying {}", source); \
  }

}  // namespace Yeager