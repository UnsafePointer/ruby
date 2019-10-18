#pragma once
#include <cstdarg>
#include <string>

std::string format(const char *fmt, va_list args);
std::string format(const char *fmt, ...);
