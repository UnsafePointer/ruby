#include "ConfigurationManager.hpp"
#include <fstream>
#include "Output.hpp"
#include <yaml/Yaml.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>

using namespace std;

const string configurationFile = "config.yaml";

ConfigurationManager::ConfigurationManager() : filePath() {}

ConfigurationManager* ConfigurationManager::instance = nullptr;

ConfigurationManager* ConfigurationManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigurationManager();
    }
    return instance;
}

void ConfigurationManager::setupConfigurationFile() {
    const string localConfigurationPath = "./" + configurationFile;
    ifstream file = ifstream(localConfigurationPath);
    if (file.good()) {
        filePath = localConfigurationPath;
        return;
    }
    printWarning("Local configuration file not found");
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    const string globalConfigurationDir = string(homedir) + "/.ruby/";
    const string globalConfigurationPath = globalConfigurationDir + configurationFile;
    file = ifstream(globalConfigurationPath);
    if (file.good()) {
        filePath = globalConfigurationPath;
        return;
    }
    printWarning("Global configuration file not found");
    printWarning("Generating default configuration at global path");
    if (mkdir(globalConfigurationDir.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            printError("Couldn't create a directory for global config with errno: %s", strerror(errno));
        }
    }
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["bios"] = "false";
    logConfigurationRef["cdrom"] = "false";
    logConfigurationRef["verbose"] = "false";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    configurationRef["debugInfoWindow"] = "false";
    configurationRef["showFramebuffer"] = "false";
    Yaml::Serialize(configuration, globalConfigurationPath.c_str());
    filePath = globalConfigurationPath;
}
