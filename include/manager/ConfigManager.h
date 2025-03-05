#pragma once
#include <string>

namespace rlx_teleport {

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    void init();
    void loadConfig();
    int getHomeLimit() const;
    void setDir(const std::string& dir);

private:
    ConfigManager() = default;

    ConfigManager(const ConfigManager&)            = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    
    int mHomeLimit = 16; 
    std::string mDir = "";
    std::string getConfigFilePath() const;
    bool mIsLoaded = false;
};

} // namespace rlx_teleport
