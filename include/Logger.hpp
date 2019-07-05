#pragma once
#include <sstream>
#include <string>

class Logger {
    static Logger* instance;

    bool shouldTrace;
    std::stringstream stream;
    uint16_t bufferSize;
    Logger();
public:
    static Logger* getInstance();
    void configure(bool enableTrace);
    void setupTraceFile();
    void logMessage(std::string message);
    void flush();
};
