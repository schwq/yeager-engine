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
#include "Common.h"
#include "Time.h"

#define YEAGER_LINUX_RED_TERMINAL_COLOR "\033[31;1m"
#define YEAGER_LINUX_YELLOW_TERMINAL_COLOR "\033[33;1m"
#define YEAGER_LINUX_GREEN_TERMINAL_COLOR "\033[32;1m"
#define YEAGER_LINUX_DEFAULT_TERMINAL_COLOR "\033[0;1m"

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
  ConsoleLogSender(const String& msg) { message = msg; }
  ConsoleLogSender(const String& msg, MessageTypeVerbosity::Enum ty, int verb, ImVec4 color)
  {
    message = msg;
    type = ty;
    verbosity = verb;
    text_color = color;
  }
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

/** in the string format of time, digits that are less that 10 appear without the zero in front of them. 
 * For example: At 09:01 am, the time will be formatted was 9:1:0, this function fixs this. 
 * When the decimal is less that 10, the function returns a string with the decimal and the zero in front, 
 * so the last example will be 09:01:00 now. 
 */
static String FormatSingleDecimalTimeDigit(Uint decimal)
{
  return (decimal < 10) ? String("0" + std::to_string(decimal)) : std::to_string(decimal);
}

static String FormatTimeString(const TimePointType& time)
{
  return String("[ " + FormatSingleDecimalTimeDigit(time.Time.Hours) + ":" +
                FormatSingleDecimalTimeDigit(time.Time.Minutes) + ":" +
                FormatSingleDecimalTimeDigit(time.Time.Seconds) + " ]");
}

/// TODO: Make the windows version of this
static void AttributeLogCompoments(int verbosity, String* symbol, String* verbose, String* color)
{
  if (verbosity == INFO) {
    *symbol = "(-) ";
    *verbose = "[INFO] ";
    *color = YEAGER_LINUX_GREEN_TERMINAL_COLOR;
  } else if (verbosity == WARNING) {
    *symbol = "(??) ";
    *verbose = "[WARN] ";
    *color = YEAGER_LINUX_YELLOW_TERMINAL_COLOR;
  } else {
    *symbol = "(!!) ";
    *verbose = "[ERROR] ";
    *color = YEAGER_LINUX_RED_TERMINAL_COLOR;
  }
}

template <typename... T>
void Log(int verbosity, fmt::format_string<T...> fmt, T&&... args)
{
  String str(fmt::format(fmt, std::forward<T>(args)...));
  ConsoleLogSender message(str, MessageTypeVerbosity::VerbosityToEnum(verbosity), verbosity,
                           VerbosityToColor(verbosity));
  String symbol, verbose, color;
  AttributeLogCompoments(verbosity, &symbol, &verbose, &color);
  message.message = String(verbose + message.message);
  gGlobalConsole.SetLogString(message);
  std::cout << color << FormatTimeString(TimePointType::CurrentTimeToTimeType()) << symbol << str << std::endl;
}

template <typename... T>
void LogDebug(int verbosity, fmt::format_string<T...> fmt, T&&... args)
{
#ifdef YEAGER_DEBUG
  Yeager::Log(verbosity, fmt, std::forward<T>(args)...);
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