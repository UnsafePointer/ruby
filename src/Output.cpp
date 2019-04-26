#include "Output.hpp"
#include <cstdarg>
#include <iostream>

using namespace std;

string format(const char *fmt, va_list args) {
    char buffer[256];

    const auto result = std::vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (result < 0) {
        return {};
    }

    const size_t length = result;
    if (length < sizeof(buffer)) {
        return { buffer, length };
    }

    std::string formatted(length, '\0');
    std::vsnprintf(formatted.data(), length + 1, fmt, args);

    return formatted;
}

void printMessage(string message) {
    std::cout << message << std::endl;
}

void printWarning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);
    printMessage(formatted);
}

void printError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    string formatted = format(fmt, args);
    va_end(args);
    printMessage(formatted);
    exit(1);
}
