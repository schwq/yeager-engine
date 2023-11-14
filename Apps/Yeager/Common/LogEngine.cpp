#include "LogEngine.h"
using namespace Yeager;

EditorConsole kConsole;

ImVec4 VerbosityToColor(int verbosity)
{
  switch (verbosity) {
    case WARNING:
      return IMGUI_YELLOW_WARNING_COLOR;
    case ERROR:
      return IMGUI_RED_ERROR_COLOR;
    case INFO:
    default:
      return IMGUI_WHITE_COMMON_COLOR;
  }
}

void EditorConsole::ReadLog()
{
  for (unsigned int x = 0; x < m_logs.size(); x++) {
    ImGui::TextColored(m_logs[x].text_color, m_logs[x].message.c_str());
  }
}