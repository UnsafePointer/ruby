#pragma once
#include <sstream>
#include <string>

class Logger {
    static Logger* instance;

    bool shouldTrace;
    bool shouldLogVerbose;
    std::stringstream stream;
    uint16_t bufferSize;
    void traceMessage(std::string message);
    Logger();
    ~Logger();
public:
    static Logger* getInstance();
    static void removeInstance();
    void configure(bool enableTrace, bool enableVerbose);
    void setupTraceFile();
    void logMessage(std::string message);
    void logWarning(std::string message);
    void logError(std::string message);
    void flush();
};
