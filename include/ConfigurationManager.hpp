#pragma once
#include <string>
#include <yaml/Yaml.hpp>
#include "Logger.hpp"

class ConfigurationManager {
    static ConfigurationManager *instance;
    Logger logger;
    std::string filePath;

    bool resizeWindowToFitFramefuffer;
    bool showDebugInfoWindow;

    LogLevel bios;
    LogLevel cdrom;
    LogLevel interconnect;
    LogLevel cpu;
    LogLevel gpu;
    bool trace;

    ConfigurationManager();
public:
    static ConfigurationManager* getInstance();

    void setupConfigurationFile();
    void loadConfiguration();

    bool shouldResizeWindowToFitFramebuffer();
    bool shouldShowDebugInfoWindow();

    LogLevel biosLogLevel();
    LogLevel cdromLogLevel();
    LogLevel interconnectLogLevel();
    LogLevel cpuLogLevel();
    LogLevel gpuLogLevel();
    bool shouldTraceLogs();
};
