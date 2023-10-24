#include "LogEngine.h"

ImVec4 VerbosityToColor(int verbosity)
{
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

yg_string ConsoleLogSenderToString(LogSource sender)
{
  switch (sender) {
    case ::kUser:
      return "USER";
    case kSystem:
      return "SYSTEM";
    case ::kNone:
    default:
      return "ERROR_SENDER";
  }
}

void EditorConsole::ReadLog()
{
  for (yg_uint x = 0; x < m_logs.size(); x++) {
    ImGui::TextColored(m_logs[x].text_color, m_logs[x].message.c_str());
  }
}