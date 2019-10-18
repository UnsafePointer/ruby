#pragma once
#include <sstream>
#include <string>
#include <limits>

enum LogLevel : uint8_t {
    NoLog = 0,
    Warning = 1,
    Message = 2,
};

LogLevel logLevelWithValue(std::string value);

class Logger {
    LogLevel level;
    bool shouldTrace;
    void traceMessage(std::string message) const;
public:
    Logger(LogLevel level);
    Logger(LogLevel level, bool shouldTrace);
    void logDebug(const char *fmt, ...) const;
    void logMessage(const char *fmt, ...) const;
    void logWarning(const char *fmt, ...) const;
    void logError(const char *fmt, ...) const;
    void flush() const;
};
