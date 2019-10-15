#include "Logger.hpp"
#include <fstream>
#include <iostream>

using namespace std;

Logger::Logger() : shouldTrace(false), stream(), bufferSize(0) {}

Logger* Logger::instance = nullptr;

Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

void Logger::configure(bool enableTrace, bool enableVerbose) {
    shouldTrace = enableTrace;
    shouldLogVerbose = enableVerbose;
}

void Logger::setupTraceFile() {
    if (!shouldTrace) {
        return;
    }
    remove("ruby.log");
}

void Logger::flush() {
    ofstream logfile = ofstream();
    logfile.open("ruby.log", ios::out | ios::app);
    logfile << this->stream.str();
    logfile.close();
    this->stream.str(string());
    this->bufferSize = 0;
}

void Logger::traceMessage(std::string message) {
    if (!shouldTrace) {
        return;
    }
    this->stream << message << endl;
    this->bufferSize += message.length();
    if (bufferSize < 8192) {
        return;
    }
    flush();
}

void Logger::logMessage(std::string message) {
    if (!shouldLogVerbose) {
        return;
    }
    cout << message << endl;
    traceMessage(message);
}

void Logger::logWarning(std::string message) {
    cout << message << endl;
    traceMessage(message);
}

void Logger::logError(std::string message) {
    cout << message << endl;
    traceMessage(message);
    flush();
}
