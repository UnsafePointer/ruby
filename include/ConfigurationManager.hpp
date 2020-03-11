#pragma once
#include <string>
#include <yaml/Yaml.hpp>
#include <filesystem>
#include "Logger.hpp"

class ConfigurationManager {
    static ConfigurationManager *instance;
    Logger logger;
    std::filesystem::path filePath;

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
    LogLevel interrupt;
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
    LogLevel interruptLogLevel();
    bool shouldTraceLogs();
};
