#pragma once
#include <string>

namespace rlx_teleport {

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    void loadConfig();
    void setDir(const std::string& dir);

    int getHomeLimit() const;
    int getTpaTimeOut() const;

private:
    ConfigManager() = default;

    ConfigManager(const ConfigManager&)            = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;


    int mHomeLimit  = 16;
    int mTpaTimeOut = 60;

    std::string mDir = "";
    std::string getConfigFilePath() const;

    bool mIsLoaded = false;
};

} // namespace rlx_teleport
