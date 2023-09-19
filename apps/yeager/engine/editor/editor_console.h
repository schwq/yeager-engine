#pragma once

#include "../../common/common.h"

enum class MessageOrigin {
  kUserOrigin,
  kSystemOrigin,
  kNoneOrigin,
  kGameOrigin
};

enum ConsoleLogSender { kUserLog, kSystemLog, kNoneLog };

struct ConsoleMessageType {
  String log;
  int verbosity;
  ImVec4 text_color;
};

extern constexpr String ConsoleLogSenderToString(ConsoleLogSender sender);
extern ImVec4 VerbosityToColor(int verbosity);

class EditorConsole {
 public:
  EditorConsole(){};
  ~EditorConsole(){};

  void SetLogString(ConsoleMessageType message) { m_logs.push_back(message); }

  void ReadLog();

 private:
  std::vector<ConsoleMessageType> m_logs;
};