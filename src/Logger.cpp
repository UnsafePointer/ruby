#include "Logger.hpp"
#include <fstream>
#include <iostream>
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

Logger::Logger(LogLevel level) : level(level) {
    ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
    shouldTrace = configurationManager->shouldTraceLogs();
}

Logger::Logger(LogLevel level, bool shouldTrace) : level(level), shouldTrace(shouldTrace) {}

void Logger::setupTraceFile() {
    if (!shouldTrace) {
        return;
    }
    remove("ruby.log");
}

void Logger::flush() const {
    ofstream logfile = ofstream();
    logfile.open("ruby.log", ios::out | ios::app);
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

void Logger::logDebug(std::string message) const {
    cout << message << endl;
    traceMessage(message);
}

void Logger::logMessage(std::string message) const {
    if (level < LogLevel::Message) {
        return;
    }
    cout << message << endl;
    traceMessage(message);
}

void Logger::logWarning(std::string message) const {
    if (level < LogLevel::Warning) {
        return;
    }
    cout << message << endl;
    traceMessage(message);
}

void Logger::logError(std::string message) const {
    cout << message << endl;
    traceMessage(message);
    flush();
    exit(1);
}
