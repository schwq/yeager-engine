#include "editor_console.h"

ImVec4 VerbosityToColor(int verbosity) {
  switch (verbosity) {
    case -1:
      return IMGUI_YELLOW_WARNING_COLOR;
    case -2:
      return IMGUI_RED_ERROR_COLOR;
    case 0:
    default:
      return IMGUI_WHITE_COMMON_COLOR;
  }
}

constexpr String ConsoleLogSenderToString(ConsoleLogSender sender) {
  switch (sender) {
    case ConsoleLogSender::kUserLog:
      return "USER";
    case ConsoleLogSender::kSystemLog:
      return "SYSTEM";
    case ConsoleLogSender::kNoneLog:
    default:
      return "ERROR_SENDER";
  }
}


void EditorConsole::ReadLog() {
  for (uint x = 0; x < m_logs.size(); x++) {
    ImGui::TextColored(m_logs[x].text_color, m_logs[x].log.c_str());
  }
}