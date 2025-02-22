#pragma once
#include "Warp.h"
#include <vector>


namespace rlx_warp {

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

    void                     init();
    const std::vector<Warp>& getWarps() const;
    Warp*                    getWarp(const std::string& name);
    WarpResult               addWarp(const Warp& warp);
    WarpResult               delWarp(const std::string& name);
    int                      getWarpCount() const;
    bool                     load();

private:
    WarpManager() = default;

    WarpManager(const WarpManager&)            = delete;
    WarpManager& operator=(const WarpManager&) = delete;

    std::vector<Warp> warps;
    void              save();
    bool              isLoaded = false;
};

} // namespace rlx_warp
