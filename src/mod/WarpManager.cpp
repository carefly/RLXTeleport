#include "WarpManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>


using namespace rlx_warp;

const std::string WARP_FILE_NAME = "warp.json";

void WarpManager::init() {
    std::ifstream file(WARP_FILE_NAME);
    if (!file.is_open()) {
        std::ofstream newFile(WARP_FILE_NAME);
        newFile.close();
    }
    load();
}

const std::vector<Warp>& WarpManager::getWarps() const { return warps; }

Warp* WarpManager::getWarp(const std::string& name) {
    for (auto& warp : warps) {
        if (warp.name == name) {
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

int WarpManager::getWarpCount() const { return (int)warps.size(); }

void WarpManager::save() {
    nlohmann::json j(warps);
    std::ofstream  file(WARP_FILE_NAME);
    file << j.dump(4);
    file.close();
}

bool WarpManager::load() {
    std::ifstream file(WARP_FILE_NAME);
    if (file.is_open()) {
        nlohmann::json j;
        try {
            file >> j;
            warps = j.get<std::vector<Warp>>();
        } catch (const std::exception& e) {
            std::cerr << "Error reading or parsing warp.json: " << e.what() << std::endl;
            warps.clear();
            file.close();
            isLoaded = false;
            return false;
        }
        file.close();
        isLoaded = true;
        return true;
    } else {
        std::cerr << "Could not open warp.json file." << std::endl;
        isLoaded = false;
        return false;
    }
}
