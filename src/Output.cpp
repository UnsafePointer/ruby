#include "Output.hpp"
#include <cstdarg>
#include "TestRunner.hpp"
#include "Logger.hpp"

using namespace std;

string format(const char *fmt, va_list args) {
    char buffer[4096];

    const auto result = vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (result < 0) {
        return {};
    }

    const size_t length = result;
    if (length < sizeof(buffer)) {
        return { buffer, length };
    }

    string formatted(length, '\0');
    vsnprintf(formatted.data(), length + 1, fmt, args);

    return formatted;
}

void printMessage(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);
    Logger *logger = Logger::getInstance();
    logger->logMessage(formatted);
}

void printWarning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);
    Logger *logger = Logger::getInstance();
    logger->logWarning(formatted);
}

void printError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);
    Logger *logger = Logger::getInstance();
    logger->logError(formatted);
    Debugger *debugger = Debugger::getInstance();
    debugger->debug();
    exit(1);
}
