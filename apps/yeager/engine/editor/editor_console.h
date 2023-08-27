#pragma once

#include "../../common/common.h"

enum class MessageOrigin {
  kUserOrigin,
  kSystemOrigin,
  kNoneOrigin,
  kGameOrigin
};

class EditorConsole {
 public:
  EditorConsole(){};
  ~EditorConsole(){};

  void SetLogString(String log) { m_logs.push_back(log); }

  void ReadLog();

 private:
  std::vector<String> m_logs;
};