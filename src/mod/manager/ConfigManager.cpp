#include "manager/ConfigManager.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace rlx_teleport;

const std::string CONFIG_FILE_NAME = "config.json";

void ConfigManager::init() {
    mDir = "";
}

void ConfigManager::loadConfig() {
    std::string configPath = getConfigFilePath();

    std::filesystem::path configFile(configPath);
    if (!std::filesystem::exists(configFile)) {

        nlohmann::json defaultConfig;
        defaultConfig["home_limit"] = mHomeLimit;

        std::filesystem::path dir = configFile.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        std::ofstream file(configPath);
        if (file.is_open()) {
            file << defaultConfig.dump(4);
            file.close();
        }

        mIsLoaded = true;
        return;
    }

    try {
        std::ifstream file(configPath);
        if (file.is_open()) {
            nlohmann::json config;
            file >> config;

            if (config.contains("home_limit")) {
                mHomeLimit = config["home_limit"].get<int>();
            }

            file.close();
            mIsLoaded = true;
        }
    } catch (const std::exception&) {
        mIsLoaded = false;
    }
}

int ConfigManager::getHomeLimit() const { return mHomeLimit; }

void ConfigManager::setDir(const std::string& dir) {
    mDir = dir;
}

std::string ConfigManager::getConfigFilePath() const {
    if (mDir.empty()) {
        return CONFIG_FILE_NAME;
    }
    return mDir + "/" + CONFIG_FILE_NAME;
}
