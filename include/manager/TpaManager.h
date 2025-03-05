#pragma once

#include <chrono>
#include <string>
#include <unordered_map>


namespace rlx_teleport {

class TpaManager {
public:
    static TpaManager& getInstance() {
        static TpaManager instance;
        return instance;
    }

    enum TpaAddRequestResult : int { success = 0, already_has_request = 1, already_has_response = 2 };
    enum TpaRequestDirection : int { goto_player = 0, come_to_me = 1 };
    struct TpaRequest {
        std::string                           from_xuid;
        std::string                           to_xuid;
        std::chrono::steady_clock::time_point request_time;
        TpaRequestDirection                   direction;
    };

    TpaAddRequestResult addTpaRequest(
        std::string const&  from_xuid,
        std::string const&  to_xuid,
        TpaRequestDirection direction = TpaRequestDirection::goto_player
    );
    bool              onTpaResponse(std::string const& to_xuid);
    const TpaRequest* findRequestByReceiver(std::string const& to_xuid);
    bool              cancelTpaRequest(std::string const& from_xuid);
    const TpaRequest* queryTpaRequest(std::string const& from_xuid);

private:
    TpaManager() = default;

    TpaManager(const TpaManager&)            = delete;
    TpaManager& operator=(const TpaManager&) = delete;

    void cleanExpiredRequests();

    bool hasActiveRequest(std::string const& from_xuid);
    bool hasActiveResponse(std::string const& to_xuid);


    std::unordered_map<std::string, TpaRequest> tpaRequests;
};

} // namespace rlx_teleport
