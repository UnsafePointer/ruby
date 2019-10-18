#pragma once
#include <string>
#include "Logger.hpp"

enum DebugSource : uint32_t {
    API = 0x8246,
    WindowSystem = 0x8247,
    ShaderCompiler = 0x8248,
    ThirdParty = 0x8249,
    Application = 0x824A,
    OtherSource = 0x824B
};

enum DebugType : uint32_t {
    Error = 0x824C,
    DeprecatedBehaviour = 0x824D,
    UndefinedBehaviour = 0x824E,
    Portability = 0x824F,
    Performance = 0x8250,
    OtherType = 0x8251
};

enum DebugSeverity : uint32_t {
    High = 0x9146,
    Medium = 0x9147,
    Low = 0x9148,
    Notification = 0x826B
};

class RendererDebugger {
    static RendererDebugger* instance;
    Logger logger;

    std::string debugSourceDescription(DebugSource source) const;
    std::string debugTypeDescription(DebugType type) const;
    std::string debugSeverityDescription(DebugSeverity severity) const;
    RendererDebugger(LogLevel logLevel);
public:
    static RendererDebugger* getInstance();

    void checkForOpenGLErrors() const;
};
