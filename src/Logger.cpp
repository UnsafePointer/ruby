#include "Logger.hpp"
#include <fstream>

using namespace std;

Logger::Logger() : shouldTrace(false), stream(), bufferSize(0) {}

Logger* Logger::instance = nullptr;

Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

void Logger::configure(bool enableTrace) {
    shouldTrace = enableTrace;
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
    this->stream.str(std::string());
    this->bufferSize = 0;
}

void Logger::logMessage(std::string message) {
    if (!shouldTrace) {
        return;
    }
    this->stream << message << std::endl;
    this->bufferSize++;
    if (bufferSize < 8192) {
        return;
    }
    flush();
}
