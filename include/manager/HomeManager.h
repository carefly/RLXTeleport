#pragma once
#include <mc/deps/core/math/Vec3.h>
#include <optional>
#include <vector>


namespace rlx_teleport {

class HomeManager {

public:
    enum HomeResult {
        LoadFailed,
        Duplicate,
        NotFound,
        Success,
        LimitExceeded,
    };

    struct HomePoint {
    public:
        std::string name;
        Vec3        pos;
        int         d;
    };

    struct DeathPoint {
    public:
        Vec3 pos;
        int  d;
    };

    static HomeManager& getInstance() {
        static HomeManager instance;
        return instance;
    }

    void                          init();
    const std::vector<HomePoint>& getHomes(std::string xuid) const;
    HomeResult addHome(const HomePoint& home, const std::string& xuid, const std::string& player_name);
    HomeResult delHome(const std::string& name, const std::string& xuid, const std::string& player_name);
    int        getHomeCount(const std::string& xuid) const;
    int        getHomeCount() const;
    int        getHomeLimit() const;
    HomeResult load(std::string& error_msg);
    void       setDir(const std::string& dir);

    void                      updateDeathPoint(const std::string& xuid, const Vec3& pos, int d);
    std::optional<DeathPoint> getDeathPoint(const std::string& xuid) const;

private:
    HomeManager() = default;

    HomeManager(const HomeManager&)            = delete;
    HomeManager& operator=(const HomeManager&) = delete;

    std::unordered_map<std::string, std::vector<HomePoint>> mHomes;
    std::unordered_map<std::string, DeathPoint>             mDeathPoints;

    void        save(std::string xuid, std::string player_name);
    bool        mIsLoaded  = false;
    int         mHomeLimit = 0;
    std::string mDir       = "";
    std::string getFileFullPath(std::string filename) const;
    std::string getXuidFromFilename(const std::string& filename) const;
};

} // namespace rlx_teleport
