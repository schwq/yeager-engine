#pragma once 

#include <format>
#include "../../common/common.h"

class EditorConsole {
    public:
    EditorConsole() {};
    ~EditorConsole() {}; 
    
    template<typename... Args>
    auto SetLog(std::format_string<Args...> fmt, Args&&... args) {
        String log = std::format(fmt, args...);
        m_logs.push_back(log);
    }
    void SetLogString(String log) {
        m_logs.push_back(log);
    }

    void ReadLog();

    private:

    std::vector<String> m_logs;
};