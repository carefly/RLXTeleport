#include "manager/WarpManager.h"
#include "common/utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>


namespace rlx_teleport {

void to_json(nlohmann::json& j, const Warp& w);
void from_json(const nlohmann::json& j, Warp& w);

void to_json(nlohmann::json& j, const Warp& w) {
    j = nlohmann::json{
        {"name",        w.name       },
        {"x",           w.x          },
        {"y",           w.y          },
        {"z",           w.z          },
        {"d",           w.d          },
        {"description", w.description}
    };
}

void from_json(const nlohmann::json& j, Warp& w) {
    w.name        = j["name"].get<std::string>();
    w.x           = j["x"].get<float>();
    w.y           = j["y"].get<float>();
    w.z           = j["z"].get<float>();
    w.d           = j["d"].get<int>();
    w.description = j["description"].get<std::string>();
}
} // namespace rlx_teleport

using namespace rlx_teleport;

const std::string WARP_FILE_NAME = "warp.json";

const std::vector<Warp>& WarpManager::getWarps() const { return warps; }

Warp* WarpManager::getWarp(const std::string& name) {
    for (auto& warp : warps) {
        if (Utils::equalsIgnoreCase(warp.name, name)) {
            return &warp;
        }
    }
    return nullptr;
}

WarpManager::WarpResult WarpManager::addWarp(const Warp& warp) {
    if (!isLoaded) {
        return WarpResult::LoadFailed;
    }

    if (getWarp(warp.name)) {
        return WarpResult::Duplicate;
    }
    warps.push_back(warp);
    save();
    return WarpResult::Success;
}

WarpManager::WarpResult WarpManager::delWarp(const std::string& name) {
    if (!isLoaded) {
        return WarpResult::LoadFailed;
    }

    for (auto it = warps.begin(); it != warps.end(); ++it) {
        if (it->name == name) {
            warps.erase(it);
            save();
            return WarpResult::Success;
        }
    }
    return WarpResult::NotFound;
}

WarpManager::WarpResult WarpManager::updateWarpDescription(const std::string& name, const std::string& description) {
    for (auto& warp : warps) {
        if (warp.name == name) {
            warp.description = description;
            save();
            return WarpResult::Success;
        }
    }
    return WarpResult::NotFound;
}

int WarpManager::getWarpCount() const { return (int)warps.size(); }

void WarpManager::save() {
    nlohmann::json j(warps);
    std::ofstream  file(getFilePath());
    file << j.dump(4);
    file.close();
}

WarpManager::WarpResult WarpManager::load(std::string& error_msg) {

    std::filesystem::path filePath = getFilePath();
    std::filesystem::path dirPath  = filePath.parent_path();

    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::ofstream newFile(filePath);
        newFile.close();
        file = std::ifstream(filePath);
    }

    if (!file.is_open()) {
        error_msg = "Could not open warp.json file";
        isLoaded  = false;
        return WarpResult::LoadFailed;
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        warps.clear();
        isLoaded  = true;
        error_msg = "";
        return WarpResult::Success;
    }
    file.seekg(0, std::ios::beg);

    nlohmann::json j = nlohmann::json::parse(file, nullptr, false);
    if (j.is_discarded()) {
        error_msg = "Invalid JSON format in warp.json";
        warps.clear();
        isLoaded = false;
        return WarpResult::LoadFailed;
    }

    if (!j.is_array() && !j.is_null()) {
        error_msg = "Invalid warp.json format: root must be an array";
        warps.clear();
        isLoaded = false;
        return WarpResult::LoadFailed;
    }

    try {
        warps     = j.is_null() ? std::vector<Warp>{} : j.get<std::vector<Warp>>();
        isLoaded  = true;
        error_msg = "";
        return WarpResult::Success;
    } catch (const std::exception& e) {
        error_msg = std::string("Error parsing warp data: ") + e.what();
        warps.clear();
        isLoaded = false;
        return WarpResult::LoadFailed;
    }
}

void WarpManager::setDir(const std::string& d) { this->mDir = d + "/warps/"; }

std::string WarpManager::getFilePath() const { return mDir + WARP_FILE_NAME; }
