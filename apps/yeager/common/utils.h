#pragma once

#include "common.h"
#include "../engine/editor/editor_console.h"

extern EditorConsole kConsole;

template <typename... T>
void YeagerLog(int verbosity, ConsoleLogSender sender,
               fmt::format_string<T...> fmt, T&&... args) {
  auto str = fmt::format(fmt, std::forward<T>(args)...);
  String log(str);

    ConsoleMessageType console_message;
    console_message.log = "[" + ConsoleLogSenderToString(sender) + "] " + log;
    console_message.text_color = VerbosityToColor(verbosity);
    console_message.verbosity = verbosity;

    kConsole.SetLogString(console_message);
  

  if (!(sender == kUserLog)) {
    VLOG_F(verbosity, log.c_str());
  }
}

extern constexpr String RemoveSuffixUntilCharacter(String expression,
                                                   char characterToStop);
extern constexpr String RemovePreffixUntilCharacter(String expression,
                                                    char characterToStop);
template <typename T>
constexpr void EraseVector(std::vector<T>* vector) {
  for (uint x = 0; x < vector.size(); x++) {
    delete vector[x];
  }
}