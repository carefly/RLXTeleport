#include "manager/HomeManager.h"
#include "manager/ConfigManager.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>


using namespace rlx_teleport;

const std::string HOME_FILE_PATH_NAME = "homes";
const int         HOME_DEFAULT_LIMIT  = 16;

void HomeManager::init() {
    mDir = HOME_FILE_PATH_NAME;
    mHomeLimit = ConfigManager::getInstance().getHomeLimit();

    // 检查目录是否存在，如果不存在则创建
    std::filesystem::path dir(mDir);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

const std::vector<HomeManager::HomePoint>& HomeManager::getHomes(std::string xuid) const {
    static const std::vector<HomePoint> emptyVector;

    auto it = mHomes.find(xuid);
    if (it != mHomes.end()) {
        return it->second;
    }
    return emptyVector;
}

HomeManager::HomeResult
HomeManager::addHome(const HomePoint& home, const std::string& xuid, const std::string& player_name) {

    if (mHomes[xuid].size() >= static_cast<size_t>(mHomeLimit)) {
        return HomeResult::LimitExceeded;
    }

    for (const auto& existingHome : mHomes[xuid]) {
        if (existingHome.name == home.name) {
            return HomeResult::Duplicate;
        }
    }

    mHomes[xuid].push_back(home);
    save(xuid, player_name);
    return HomeResult::Success;
}

HomeManager::HomeResult
HomeManager::delHome(const std::string& name, const std::string& xuid, const std::string& player_name) {
    auto it = mHomes.find(xuid);
    if (it == mHomes.end()) {
        return HomeResult::NotFound;
    }

    auto& homes = it->second;
    for (auto homeIt = homes.begin(); homeIt != homes.end(); ++homeIt) {
        if (homeIt->name == name) {
            homes.erase(homeIt);
            save(xuid, player_name);
            return HomeResult::Success;
        }
    }

    return HomeResult::NotFound;
}

int HomeManager::getHomeCount(const std::string& xuid) const {
    auto it = mHomes.find(xuid);
    if (it != mHomes.end()) {
        return (int)it->second.size();
    }
    return 0;
}

int HomeManager::getHomeCount() const {
    int count = 0;
    for (const auto& [xuid, homes] : mHomes) {
        count += (int)homes.size();
    }
    return count;
}

int HomeManager::getHomeLimit() const { return mHomeLimit; }

std::string HomeManager::getXuidFromFilename(const std::string& filename) const {
    size_t dashPos = filename.find(" - ");
    if (dashPos == std::string::npos) {
        return "";
    }
    return filename.substr(0, dashPos);
}

HomeManager::HomeResult HomeManager::load(std::string& error_msg) {
    mHomes.clear();

    std::string dirPath = mDir;

    std::filesystem::path dir(dirPath);
    if (!std::filesystem::exists(dir)) {
        if (!std::filesystem::create_directories(dir)) {
            error_msg = "Failed to create directory: " + dirPath;
            return HomeResult::LoadFailed;
        }
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        std::string filename = entry.path().filename().string();
        std::string xuid     = getXuidFromFilename(filename);

        if (xuid.empty()) {
            continue;
        }

        std::ifstream file(entry.path());
        if (!file.is_open()) {
            continue;
        }

        try {
            nlohmann::json json;
            file >> json;

            std::vector<HomePoint> homes;
            for (const auto& homeJson : json) {
                HomePoint home;
                home.name  = homeJson["name"];
                home.pos.x = homeJson["pos"]["x"];
                home.pos.y = homeJson["pos"]["y"];
                home.pos.z = homeJson["pos"]["z"];
                home.d     = homeJson["d"];
                homes.push_back(home);
            }

            mHomes[xuid] = homes;
        } catch (const std::exception& e) {
            error_msg = "Failed to load home: " + entry.path().string() + " " + e.what();
            continue;
        }
    }

    mIsLoaded = true;
    return HomeResult::Success;
}

void HomeManager::save(std::string xuid, std::string player_name) {
    std::string dirPath     = mDir;
    std::string newFilePath = getFileFullPath(xuid + " - " + player_name);

    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        std::string filename = entry.path().filename().string();
        std::string fileXuid = getXuidFromFilename(filename);
        if (fileXuid.empty()) {
            continue;
        }

        if (fileXuid == xuid) {
            std::filesystem::remove(entry.path());
        }
    }

    nlohmann::json homesJson = nlohmann::json::array();

    for (const auto& home : mHomes[xuid]) {
        nlohmann::json homeJson;
        homeJson["name"]     = home.name;
        homeJson["pos"]["x"] = home.pos.x;
        homeJson["pos"]["y"] = home.pos.y;
        homeJson["pos"]["z"] = home.pos.z;
        homeJson["d"]        = home.d;
        homesJson.push_back(homeJson);
    }

    std::ofstream file(newFilePath);
    if (file.is_open()) {
        file << homesJson.dump(4);
    }
}

std::string HomeManager::getFileFullPath(std::string filename) const { return mDir + "/" + filename + ".json"; }

void HomeManager::updateDeathPoint(const std::string& xuid, const Vec3& pos, int d) {
    mDeathPoints[xuid] = DeathPoint{pos, d};
}

std::optional<HomeManager::DeathPoint> HomeManager::getDeathPoint(const std::string& xuid) const {
    auto it = mDeathPoints.find(xuid);
    if (it != mDeathPoints.end()) {
        return it->second;
    }
    return std::nullopt;
}

void HomeManager::setDir(const std::string& dir) { mDir = dir + "/" + HOME_FILE_PATH_NAME; }
