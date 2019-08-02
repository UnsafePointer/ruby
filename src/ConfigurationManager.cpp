#include "ConfigurationManager.hpp"

ConfigurationManager::ConfigurationManager() {}

ConfigurationManager* ConfigurationManager::instance = nullptr;

ConfigurationManager* ConfigurationManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigurationManager();
    }
    return instance;
}
