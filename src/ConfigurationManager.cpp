#include "ConfigurationManager.hpp"
#include <fstream>
#include "Output.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>

using namespace std;

const string configurationFile = "config.yaml";

ConfigurationManager::ConfigurationManager() : logger(LogLevel::Warning, false), filePath(), resizeWindowToFitFramefuffer(false), showDebugInfoWindow(false),  bios(NoLog), cdrom(NoLog), interconnect(NoLog), cpu(NoLog), gpu(NoLog), trace(false) {}

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
    logger.logWarning("Local configuration file not found");
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
    logger.logWarning("Global configuration file not found");
    logger.logWarning("Generating default configuration at global path");
    if (mkdir(globalConfigurationDir.c_str(), 0777) == -1) {
        if (errno != EEXIST) {
            logger.logError(format("Couldn't create a directory for global config with errno: %s", strerror(errno)));
        }
    }
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["bios"] = "OFF";
    logConfigurationRef["cdrom"] = "OFF";
    logConfigurationRef["interconnect"] = "OFF";
    logConfigurationRef["cpu"] = "OFF";
    logConfigurationRef["gpu"] = "OFF";
    logConfigurationRef["trace"] = "false";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    configurationRef["debugInfoWindow"] = "false";
    configurationRef["showFramebuffer"] = "false";
    Yaml::Serialize(configuration, globalConfigurationPath.c_str());
    filePath = globalConfigurationPath;
}

void ConfigurationManager::loadConfiguration() {
    Yaml::Node configuration = Yaml::Node();
    Yaml::Parse(configuration, filePath.c_str());
    resizeWindowToFitFramefuffer = configuration["showFramebuffer"].As<bool>();
    showDebugInfoWindow = configuration["debugInfoWindow"].As<bool>();
    bios = logLevelWithValue(configuration["log"]["bios"].As<string>());
    cdrom = logLevelWithValue(configuration["log"]["cdrom"].As<string>());
    interconnect = logLevelWithValue(configuration["log"]["interconnect"].As<string>());
    cpu = logLevelWithValue(configuration["log"]["cpu"].As<string>());
    gpu = logLevelWithValue(configuration["log"]["gpu"].As<string>());
    trace = configuration["log"]["trace"].As<bool>();
}

bool ConfigurationManager::shouldResizeWindowToFitFramebuffer() {
    return resizeWindowToFitFramefuffer;
}

bool ConfigurationManager::shouldShowDebugInfoWindow() {
    return showDebugInfoWindow;
}

LogLevel ConfigurationManager::biosLogLevel() {
    return bios;
}

LogLevel ConfigurationManager::cdromLogLevel() {
    return cdrom;
}

LogLevel ConfigurationManager::interconnectLogLevel() {
    return interconnect;
}

LogLevel ConfigurationManager::cpuLogLevel() {
    return cpu;
}

LogLevel ConfigurationManager::gpuLogLevel() {
    return gpu;
}

bool ConfigurationManager::shouldTraceLogs() {
    return trace;
}
