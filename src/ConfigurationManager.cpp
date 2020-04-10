#include "ConfigurationManager.hpp"
#include <fstream>

using namespace std;

const string configurationFile = "config.yaml";

ConfigurationManager::ConfigurationManager() : logger(LogLevel::Warning, "", false), filePath(filesystem::current_path() / configurationFile), ctrllerName(""), resizeWindowToFitFramefuffer(false), showDebugInfoWindow(false),  bios(NoLog), cdrom(NoLog), interconnect(NoLog), cpu(NoLog), gpu(NoLog), opengl(NoLog), dma(NoLog), controller(NoLog), interrupt(NoLog), trace(false) {}

ConfigurationManager* ConfigurationManager::instance = nullptr;

ConfigurationManager* ConfigurationManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigurationManager();
    }
    return instance;
}

void ConfigurationManager::setupConfigurationFile() {
    ifstream file = ifstream(filePath);
    if (file.good()) {
        return;
    }
    logger.logWarning("Local configuration file not found");
    Yaml::Node logConfiguration = Yaml::Node();
    Yaml::Node &logConfigurationRef = logConfiguration;
    logConfigurationRef["bios"] = "NOLOG";
    logConfigurationRef["cdrom"] = "NOLOG";
    logConfigurationRef["interconnect"] = "NOLOG";
    logConfigurationRef["cpu"] = "NOLOG";
    logConfigurationRef["gpu"] = "NOLOG";
    logConfigurationRef["opengl"] = "NOLOG";
    logConfigurationRef["dma"] = "NOLOG";
    logConfigurationRef["spu"] = "NOLOG";
    logConfiguration["controller"] = "NOLOG";
    logConfiguration["interrupt"] = "NOLOG";
    logConfiguration["gte"] = "NOLOG";
    logConfigurationRef["trace"] = "false";
    Yaml::Node configuration = Yaml::Node();
    Yaml::Node &configurationRef = configuration;
    configurationRef["log"] = logConfiguration;
    configurationRef["controllerName"] = "Sony Interactive Entertainment Controller";
    configurationRef["debugInfoWindow"] = "false";
    configurationRef["showFramebuffer"] = "false";
    Yaml::Serialize(configuration, filePath.string().c_str());
}

void ConfigurationManager::loadConfiguration() {
    Yaml::Node configuration = Yaml::Node();
    Yaml::Parse(configuration, filePath.string().c_str());
    ctrllerName = configuration["controllerName"].As<string>();
    resizeWindowToFitFramefuffer = configuration["showFramebuffer"].As<bool>();
    showDebugInfoWindow = configuration["debugInfoWindow"].As<bool>();
    bios = logLevelWithValue(configuration["log"]["bios"].As<string>());
    cdrom = logLevelWithValue(configuration["log"]["cdrom"].As<string>());
    interconnect = logLevelWithValue(configuration["log"]["interconnect"].As<string>());
    cpu = logLevelWithValue(configuration["log"]["cpu"].As<string>());
    gpu = logLevelWithValue(configuration["log"]["gpu"].As<string>());
    opengl = logLevelWithValue(configuration["log"]["opengl"].As<string>());
    dma = logLevelWithValue(configuration["log"]["dma"].As<string>());
    spu = logLevelWithValue(configuration["log"]["spu"].As<string>());
    controller = logLevelWithValue(configuration["log"]["controller"].As<string>());
    interrupt = logLevelWithValue(configuration["log"]["interrupt"].As<string>());
    gte = logLevelWithValue(configuration["log"]["gte"].As<string>());
    trace = configuration["log"]["trace"].As<bool>();
    if (trace) {
        remove("ruby.log");
    }
}

std::string ConfigurationManager::controllerName() {
    return ctrllerName;
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

LogLevel ConfigurationManager::openGLLogLevel() {
    return opengl;
}

LogLevel ConfigurationManager::dmaLogLevel() {
    return dma;
}

LogLevel ConfigurationManager::spuLogLevel() {
    return spu;
}

LogLevel ConfigurationManager::controllerLogLevel() {
    return controller;
}

LogLevel ConfigurationManager::interruptLogLevel() {
    return interrupt;
}

LogLevel ConfigurationManager::gteLogLevel() {
    return gte;
}

bool ConfigurationManager::shouldTraceLogs() {
    return trace;
}
