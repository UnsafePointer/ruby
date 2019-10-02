#pragma once
#include <string>
#include <yaml/Yaml.hpp>

class ConfigurationManager {
    static ConfigurationManager *instance;
    std::string filePath;
    Yaml::Node configuration;

    ConfigurationManager();
    ~ConfigurationManager();
public:
    static ConfigurationManager* getInstance();
    static void removeInstance();

    void setupConfigurationFile();
    void loadConfiguration();
    bool shouldResizeWindowToFitFramebuffer();
    bool shouldLogVerbose();
    bool shouldShowDebugInfoWindow();
    bool shouldLogBiosFunctionCalls();
    bool shouldLogCDROMActivity();
    bool shouldTraceLogs();
};
