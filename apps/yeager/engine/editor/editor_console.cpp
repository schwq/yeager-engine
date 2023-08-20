#include "editor_console.h"


void EditorConsole::ReadLog() {
    for(uint x = 0; x < m_logs.size(); x++) {
        ImGui::Text(m_logs[x].c_str());
    }
}