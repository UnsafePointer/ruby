#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "Output.hpp"
#include "ConfigurationManager.hpp"

using namespace std;

const uint32_t BUFFER_SIZE_LIMIT = 8192;

std::stringstream stream = std::stringstream();
uint16_t bufferSize = 0;

LogLevel logLevelWithValue(std::string value) {
    if (value.compare("WAR") == 0) {
        return LogLevel::Warning;
    } else if (value.compare("MSG") == 0) {
        return LogLevel::Message;
    } else if (value.compare("NOLOG") == 0) {
        return LogLevel::NoLog;
    }
    return LogLevel::NoLog;
}

Logger::Logger(LogLevel level) : level(level), prefix("") {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    shouldTrace = configurationManager->shouldTraceLogs();
}

Logger::Logger(LogLevel level, string prefix) : level(level), prefix(prefix) {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    shouldTrace = configurationManager->shouldTraceLogs();
}

Logger::Logger(LogLevel level, string prefix, bool shouldTrace) : level(level), prefix(prefix), shouldTrace(shouldTrace) {}

void Logger::flush() const {
    ofstream logfile = ofstream();
    filesystem::path logFilePath = filesystem::current_path() / "ruby.log";
    logfile.open(logFilePath, ios::out | ios::app);
    logfile << stream.str();
    logfile.close();
    stream.str(string());
    bufferSize = 0;
}

void Logger::traceMessage(std::string message) const {
    if (!shouldTrace) {
        return;
    }
    stream << message << endl;
    bufferSize += message.length();
    if (bufferSize < BUFFER_SIZE_LIMIT) {
        return;
    }
    flush();
}

void Logger::logDebug(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    cout << formatted << endl;
    traceMessage(formatted);
}

void Logger::logMessage(const char *fmt, ...) const {
    if (level < LogLevel::Message) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    cout << formatted << endl;
    traceMessage(formatted);
}

void Logger::logWarning(const char *fmt, ...) const {
    if (level < LogLevel::Warning) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    cout << formatted << endl;
    traceMessage(formatted);
}

void Logger::logError(const char *fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);

    formatted.insert(0, prefix);
    cout << formatted << endl;
    traceMessage(formatted);
    flush();
    exit(1);
}
