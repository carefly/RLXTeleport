#pragma once
#include <string>
#include <vector>


namespace rlx_teleport {

struct Warp {
    std::string name;
    float       x;
    float       y;
    float       z;
    int         d;
    std::string description;
};

class WarpManager {

public:
    enum WarpResult {
        LoadFailed,
        Duplicate,
        NotFound,
        Success,
    };

    static WarpManager& getInstance() {
        static WarpManager instance;
        return instance;
    }

    const std::vector<Warp>& getWarps() const;
    Warp*                    getWarp(const std::string& name);
    WarpResult               addWarp(const Warp& warp);
    WarpResult               delWarp(const std::string& name);
    WarpResult               updateWarpDescription(const std::string& name, const std::string& description);
    int                      getWarpCount() const;
    WarpResult               load(std::string& error_msg);
    void                     setDir(const std::string& dir);

private:
    WarpManager() = default;

    WarpManager(const WarpManager&)            = delete;
    WarpManager& operator=(const WarpManager&) = delete;

    std::vector<Warp> warps;
    void              save();
    bool              isLoaded = false;
    std::string       mDir     = "";
    std::string       getFilePath() const;
};

} // namespace rlx_teleport
