#pragma once
#include <string>

class ConfigurationManager {
    static ConfigurationManager *instance;
    std::string filePath;

    ConfigurationManager();
public:
    static ConfigurationManager* getInstance();

    void setupConfigurationFile();
};
