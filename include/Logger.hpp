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
    void setupTraceFile();
    void logDebug(std::string message) const;
    void logMessage(std::string message) const;
    void logWarning(std::string message) const;
    void logError(std::string message) const;
    void flush() const;
};
