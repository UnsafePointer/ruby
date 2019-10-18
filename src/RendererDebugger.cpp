#include "RendererDebugger.hpp"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <sstream>
#include "ConfigurationManager.hpp"

using namespace std;

RendererDebugger::RendererDebugger(LogLevel logLevel) : logger(logLevel) {}

RendererDebugger* RendererDebugger::instance = nullptr;

RendererDebugger* RendererDebugger::getInstance() {
    if (instance == nullptr) {
        ConfigurationManager *configurationManager = ConfigurationManager::getInstance();
        instance = new RendererDebugger(configurationManager->openGLLogLevel());
    }
    return instance;
}

string RendererDebugger::debugSourceDescription(DebugSource source) const {
    switch (source) {
        case API: {
            return "API";
        }
        case WindowSystem: {
            return "WindowSystem";
        }
        case ShaderCompiler: {
            return "ShaderCompiler";
        }
        case ThirdParty: {
            return "ThirdParty";
        }
        case Application: {
            return "Application";
        }
        case OtherSource: {
            return "Other";
        }
        default: {
            logger.logError("Invalid source");
            return "";
        }
    }
}

string RendererDebugger::debugTypeDescription(DebugType type) const {
    switch (type) {
        case Error: {
            return "Error";
        }
        case DeprecatedBehaviour: {
            return "DeprecatedBehaviour";
        }
        case UndefinedBehaviour: {
            return "UndefinedBehaviour";
        }
        case Portability: {
            return "Portability";
        }
        case Performance: {
            return "Performance";
        }
        case OtherType: {
            return "Other";
        }
        default: {
            logger.logError("Invalid type");
            return "";
        }
    }
}

string RendererDebugger::debugSeverityDescription(DebugSeverity severity) const {
    switch (severity) {
        case High: {
            return "High";
        }
        case Medium: {
            return "Medium";
        }
        case Low: {
            return "Low";
        }
        case Notification: {
            return "Notification";
        }
        default: {
            logger.logError("Invalid severity");
            return "";
        }
    }
}

void RendererDebugger::checkForOpenGLErrors() const {
    bool highSeverityFound = false;
    while (true) {
        vector<GLchar> buffer(4096);
        GLenum severity;
        GLenum source;
        GLsizei messageSize;
        GLenum type;
        GLenum id;
        GLuint count = glGetDebugMessageLog(1, (GLsizei)buffer.size(), &source, &type, &id, &severity, &messageSize, &buffer[0]);
        if (count == 0) {
            break;
        }
        buffer.resize(messageSize);
        string message = string(buffer.begin(), buffer.end());
        DebugSource debugSource = DebugSource(source);
        DebugType debugType = DebugType(type);
        DebugSeverity debugSeverity = DebugSeverity(severity);

        ostringstream stream;
        stream << "OpenGL: [" << debugSeverityDescription(debugSeverity)
             << "|" << debugSourceDescription(debugSource)
             << "|" << debugTypeDescription(debugType)
             << "|0x" << hex << id << "|"
             << message;
        logger.logWarning(stream.str().c_str());
        if (debugSeverity == DebugSeverity::High) {
            highSeverityFound = true;
        }
    }
    if (highSeverityFound) {
        logger.logError("Encountered high severity OpenGL error");
    }
}
