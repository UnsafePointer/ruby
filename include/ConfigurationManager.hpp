#pragma once
#include <string>
#include <yaml/Yaml.hpp>

class ConfigurationManager {
    static ConfigurationManager *instance;
    std::string filePath;
    Yaml::Node configuration;

    ConfigurationManager();
public:
    static ConfigurationManager* getInstance();

    void setupConfigurationFile();
    void loadConfiguration();
    bool shouldResizeWindowToFitFramebuffer();
    bool shouldLogVerbose();
    bool shouldShowDebugInfoWindow();
    bool shouldLogBiosFunctionCalls();
    bool shouldLogCDROMActivity();
};
