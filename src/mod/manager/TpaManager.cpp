#include "manager/TpaManager.h"
#include "manager/ConfigManager.h"

using namespace rlx_teleport;

TpaManager::TpaAddRequestResult
TpaManager::addTpaRequest(std::string const& from_xuid, std::string const& to_xuid, TpaRequestDirection direction) {
    cleanExpiredRequests();

    if (hasActiveRequest(from_xuid)) {
        return TpaAddRequestResult::already_has_request;
    }

    if (hasActiveResponse(to_xuid)) {
        return TpaAddRequestResult::already_has_response;
    }

    TpaRequest request{from_xuid, to_xuid, std::chrono::steady_clock::now(), direction};
    tpaRequests[from_xuid] = request;

    return TpaAddRequestResult::success;
}

bool TpaManager::onTpaResponse(std::string const& to_xuid) {

    cleanExpiredRequests();

    auto req = findRequestByReceiver(to_xuid);
    if (req == nullptr) {
        return false;
    }

    tpaRequests.erase(req->from_xuid);

    return true;
}

void TpaManager::cleanExpiredRequests() {
    auto now = std::chrono::steady_clock::now();

    auto it = tpaRequests.begin();
    while (it != tpaRequests.end()) {
        const auto& request  = it->second;
        auto        duration = std::chrono::duration_cast<std::chrono::seconds>(now - request.request_time).count();
        if (duration >= ConfigManager::getInstance().getTpaTimeOut()) {
            it = tpaRequests.erase(it);
        } else {
            ++it;
        }
    }
}

bool TpaManager::hasActiveRequest(std::string const& from_xuid) {
    return tpaRequests.find(from_xuid) != tpaRequests.end();
}

bool TpaManager::hasActiveResponse(std::string const& to_xuid) {
    auto req = findRequestByReceiver(to_xuid);
    return req != nullptr;
}

const TpaManager::TpaRequest* TpaManager::findRequestByReceiver(std::string const& to_xuid) {
    cleanExpiredRequests();
    for (const auto& pair : tpaRequests) {
        if (pair.second.to_xuid == to_xuid) {
            return &(pair.second);
        }
    }
    return nullptr;
}

bool TpaManager::cancelTpaRequest(std::string const& from_xuid) {
    auto it = tpaRequests.find(from_xuid);
    if (it == tpaRequests.end()) {
        return false;
    }
    tpaRequests.erase(it);
    return true;
}

const TpaManager::TpaRequest* TpaManager::queryTpaRequest(std::string const& from_xuid) {
    cleanExpiredRequests();
    auto it = tpaRequests.find(from_xuid);
    if (it == tpaRequests.end()) {
        return nullptr;
    }
    return &(it->second);
}
