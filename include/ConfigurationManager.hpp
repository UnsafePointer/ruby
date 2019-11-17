#pragma once
#include <string>
#include <yaml/Yaml.hpp>
#include "Logger.hpp"

class ConfigurationManager {
    static ConfigurationManager *instance;
    Logger logger;
    std::string filePath;

    std::string ctrllerName;
    bool resizeWindowToFitFramefuffer;
    bool showDebugInfoWindow;

    LogLevel bios;
    LogLevel cdrom;
    LogLevel interconnect;
    LogLevel cpu;
    LogLevel gpu;
    LogLevel opengl;
    LogLevel dma;
    LogLevel spu;
    LogLevel controller;
    bool trace;

    ConfigurationManager();
public:
    static ConfigurationManager* getInstance();

    void setupConfigurationFile();
    void loadConfiguration();

    std::string controllerName();
    bool shouldResizeWindowToFitFramebuffer();
    bool shouldShowDebugInfoWindow();

    LogLevel biosLogLevel();
    LogLevel cdromLogLevel();
    LogLevel interconnectLogLevel();
    LogLevel cpuLogLevel();
    LogLevel gpuLogLevel();
    LogLevel openGLLogLevel();
    LogLevel dmaLogLevel();
    LogLevel spuLogLevel();
    LogLevel controllerLogLevel();
    bool shouldTraceLogs();
};
