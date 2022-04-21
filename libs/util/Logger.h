#pragma once
#include "nocopyable.h"
#include <string>

enum LogLevel
{
    DEBUG,
    INFO,
    WARN,
    FATAL
};

class Logger : public nocopyable
{
public:
    Logger() = default;
    Logger(LogLevel level);
    ~Logger() = default;

    static Logger& Instance();

public:
    void Log(LogLevel level, const std::string& content);

public:
    void SetLevel(LogLevel level) { m_level = level; }
    LogLevel GetLevel() { return m_level; }

private:
    LogLevel m_level = DEBUG;
};

#define LOG_DEBUG(LOGFORMAT, ...)                       \
    do                                                  \
    {                                                   \
        char buff[4096];                                \
        snprintf(buff, 4096, LOGFORMAT, ##__VA_ARGS__); \
        Logger::Instance().Log(DEBUG, buff);            \
    } while (0)

#define LOG_INFO(LOGFORMAT, ...)                        \
    do                                                  \
    {                                                   \
        char buff[4096];                                \
        snprintf(buff, 4096, LOGFORMAT, ##__VA_ARGS__); \
        Logger::Instance().Log(INFO, buff);             \
    } while (0)

#define LOG_WARN(LOGFORMAT, ...)                        \
    do                                                  \
    {                                                   \
        char buff[4096];                                \
        snprintf(buff, 4096, LOGFORMAT, ##__VA_ARGS__); \
        Logger::Instance().Log(WARN, buff);             \
    } while (0)

#define LOG_FATAL(LOGFORMAT, ...)                       \
    do                                                  \
    {                                                   \
        char buff[4096];                                \
        snprintf(buff, 4096, LOGFORMAT, ##__VA_ARGS__); \
        Logger::Instance().Log(FATAL, buff);            \
    } while (0)                           
                 