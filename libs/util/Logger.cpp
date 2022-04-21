#include "Logger.h"
#include <iostream>

Logger::Logger(LogLevel level)
: m_level(level)
{
}

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

void Logger::Log(LogLevel level, const std::string& content)
{
    if (level < m_level)
        return;
        
    switch(level)
    {
        case DEBUG:
            std::cout << "[Debug] " << content << std::endl;
            break;
        case INFO:
            std::cout << "[Info] " << content << std::endl;
             break;
        case WARN:
            std::cout << "[Warn] " << content << std::endl;
             break;
        case FATAL:
            std::cout << "[Fatal] " << content << std::endl;
             break;
    }
}