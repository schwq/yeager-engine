#pragma once 

#include <format>
#include "../../common/common.h"

class EditorConsole {
    public:
    EditorConsole() {};
    ~EditorConsole() {}; 
    void SetLog(String log) {
        m_logs.push_back(log);
    }

    void ReadLog();

    private:

    std::vector<String> m_logs;
};