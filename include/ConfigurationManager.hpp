#pragma once

class ConfigurationManager {
    static ConfigurationManager *instance;

    ConfigurationManager();
public:
    static ConfigurationManager* getInstance();
};
