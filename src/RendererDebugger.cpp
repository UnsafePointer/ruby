#include "RendererDebugger.hpp"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum DebugSource {
    API = 0x8246,
    WindowSystem = 0x8247,
    ShaderCompiler = 0x8248,
    ThirdParty = 0x8249,
    Application = 0x824A,
    OtherSource = 0x824B
};

string debugSourceDescription(DebugSource source) {
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
            cout << "Invalid source" << endl;
            exit(1);
        }
    }
}

enum DebugType {
    Error = 0x824C,
    DeprecatedBehaviour = 0x824D,
    UndefinedBehaviour = 0x824E,
    Portability = 0x824F,
    Performance = 0x8250,
    OtherType = 0x8251
};

string debugTypeDescription(DebugType type) {
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
            cout << "Invalid type" << endl;
            exit(1);
        }
    }
}

enum DebugSeverity {
    High = 0x9146,
    Medium = 0x9147,
    Low = 0x9148,
    Notification = 0x826B
};

string debugSeverityDescription(DebugSeverity severity) {
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
            cout << "Invalid severity" << endl;
            exit(1);
        }
    }
}

void checkForOpenGLErrors() {
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

        cout << "OpenGL: [" << debugSeverityDescription(debugSeverity)
             << "|" << debugSourceDescription(debugSource)
             << "|" << debugTypeDescription(debugType)
             << "|0x" << hex << id << "|"
             << message << endl;
        if (debugSeverity == DebugSeverity::High) {
            highSeverityFound = true;
        }
    }
    if (highSeverityFound) {
        cout << "Encountered high severity OpenGL error" << endl;
        exit(1);
    }
}
